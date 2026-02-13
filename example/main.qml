import QtQuick 2.15
import QtQuick.Window 2.15
import QtPositioning 5.12
import QtLocation 5.12
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Hello World")

    ColumnLayout {
        anchors.fill: parent

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 50
            color: "gray"

            RowLayout {
                anchors.fill: parent

                Item {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }

                ComboBox {
                    id: mapTypeComboBox

                    Layout.preferredWidth: 200
                    Layout.fillHeight: true



                    model: ListModel {
                        id: comboBoxModel
                    }

                    onCurrentTextChanged: {
                        for (var i = 0; i < map.supportedMapTypes.length; ++i) {
                            var type = map.supportedMapTypes[i];
                            if (type.name === mapTypeComboBox.currentText) {
                                map.activeMapType = type
                                console.log("set active map to: " + type.name)
                            }
                        }
                    }
                }
            }
        }

        Map {
            id: map

            Layout.fillWidth: true
            Layout.fillHeight: true

            plugin: Plugin {
                id: plugin
                name: "amap";

                parameters: [
                    PluginParameter { name: "amap.offline"; value: true }
                ]
            }

            Component.onCompleted: {
                for (var i = 0; i < map.supportedMapTypes.length; ++i) {
                    var type = map.supportedMapTypes[i];
                    console.log("supported map type: " + type.name)
                    comboBoxModel.append({text: type.name});
                    if (type.name === "Terrain") {
                        map.activeMapType = type
                        mapTypeComboBox.currentIndex = i
                    }
                }
            }
        }
    }




}
