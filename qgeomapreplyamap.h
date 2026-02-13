#ifndef QGEOMAPREPLYAMAP_H
#define QGEOMAPREPLYAMAP_H

#include <QtNetwork/QNetworkReply>
#include <QtLocation/private/qgeotilespec_p.h>
#include <QtLocation/private/qgeotiledmapreply_p.h>
#include <QtCore/QPointer>
#include <QSqlDatabase>

QT_BEGIN_NAMESPACE

class QGeoMapReplyAmap : public QGeoTiledMapReply
{
    Q_OBJECT

public:
    explicit QGeoMapReplyAmap(QNetworkReply *reply, const QGeoTileSpec &spec, QObject *parent = 0);
    QGeoMapReplyAmap(QSqlDatabase &mbtiles, const QByteArray &pic404, const QGeoTileSpec &spec, QObject *parent = 0);

    ~QGeoMapReplyAmap();

    void abort();

    QNetworkReply *networkReply() const;

private Q_SLOTS:
    void networkFinished();
    void networkError(QNetworkReply::NetworkError error);

private:
    QPointer<QNetworkReply> m_reply;
};

QT_END_NAMESPACE

#endif
