import QtQuick 2.7
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.1
import QtQuick.Window 2.2

import three.quick 1.0

Window {
    id: mainWindow
    minimumWidth: 1280
    minimumHeight: 1024

    visible: true

    ThreeDItem {
        id: threeD
        z: 1
        anchors.fill: parent
        focus: true
        background: Qt.rgba(0.4,0.4,0.6)
    }
}
