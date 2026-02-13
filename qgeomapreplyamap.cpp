#include "qgeomapreplyamap.h"
#include <QNetworkAccessManager>
#include <QNetworkCacheMetaData>
#include <QDateTime>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QTimer>
#include <QSqlDatabase>
#include <QThread>


QT_BEGIN_NAMESPACE

QGeoMapReplyAmap::QGeoMapReplyAmap(QNetworkReply *reply, const QGeoTileSpec &spec, QObject *parent)
    : QGeoTiledMapReply(spec, parent),
    m_reply(reply)
{
    connect(m_reply,
            SIGNAL(finished()),
            this,
            SLOT(networkFinished()));

    connect(m_reply,
            SIGNAL(error(QNetworkReply::NetworkError)),
            this,
            SLOT(networkError(QNetworkReply::NetworkError)));
}

QGeoMapReplyAmap::QGeoMapReplyAmap(QSqlDatabase &mbtiles, const QByteArray &pic404, const QGeoTileSpec &spec, QObject *parent)
    : QGeoTiledMapReply(spec, parent)
{
    if (!mbtiles.open()) {
        QTimer::singleShot(100, this, [this]() { setFinished(true); });
        return;
    }
    QSqlQuery queryTileFormat(mbtiles);
    queryTileFormat.prepare(R"(SELECT value FROM metadata WHERE name='format')");
    if (!queryTileFormat.exec()) {
        QTimer::singleShot(0, this, [this]() { setFinished(true); });
        return;
    }
    if (queryTileFormat.next()) {
        setMapImageFormat(queryTileFormat.value(0).toString());
    } else {
        setMapImageFormat("png");
    }
    int z = tileSpec().zoom();
    int y = tileSpec().y();
    int x = tileSpec().x();
    int tmsY = (1 << z) - 1 - y;
    QSqlQuery queryTileData(mbtiles);
    queryTileData.prepare(R"(SELECT tile_data FROM tiles WHERE zoom_level=? AND tile_column=? AND tile_row=?)");
    queryTileData.addBindValue(z);
    queryTileData.addBindValue(x);
    queryTileData.addBindValue(tmsY);
    if (!queryTileData.exec()) {
        QTimer::singleShot(0, this, [this]() { setFinished(true); });
        return;
    }
    if (queryTileData.next()) {
        setMapImageData(queryTileData.value(0).toByteArray());
        QTimer::singleShot(0, this, [this]() { setFinished(true); });
        return;
    }
    /* 没有找到就填充一个404图片 */
    setMapImageData(pic404);
    QTimer::singleShot(0, this, [this]() { setFinished(true); });
}



QGeoMapReplyAmap::~QGeoMapReplyAmap()
{
}

QNetworkReply *QGeoMapReplyAmap::networkReply() const
{
    return m_reply;
}

void QGeoMapReplyAmap::abort()
{
    if (!m_reply)
        return;

    m_reply->abort();
}

void QGeoMapReplyAmap::networkFinished()
{
    if (!m_reply)
        return;

    if (m_reply->error() != QNetworkReply::NoError)
        return;

    setMapImageData(m_reply->readAll());
    const int _mid = tileSpec().mapId();
    if (_mid == 2)
        setMapImageFormat("jpeg");
    else
        setMapImageFormat("png");
    setFinished(true);

    m_reply->deleteLater();
    m_reply = 0;
}

void QGeoMapReplyAmap::networkError(QNetworkReply::NetworkError error)
{
    Q_UNUSED(error);
    if (!m_reply)
        return;

    setFinished(true);
    setCached(false);
    m_reply->deleteLater();
    m_reply = 0;
}

QT_END_NAMESPACE
