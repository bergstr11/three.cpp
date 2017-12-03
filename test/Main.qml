import QtQuick 2.7
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.1
import QtQuick.Window 2.2

import com.lightningobjects.ui 1.0

Window {
    id: mainWindow
    minimumWidth: 1280
    minimumHeight: 1024

    visible: true

    ThreeDItem {
        z: 1
        anchors.fill: parent
        focus: true
        background: "yellow"
        data: "/home/byter/work/git/vtk_qml/data/Mercedes_GLE6/gle63amg_lowpoly.3DS"
    }
    Text {
        anchors.centerIn: parent
        font.pointSize: 50
        text: "Hello ThreePePe"
        visible: true
        color: "yellow"
        opacity: 0.5
        z: 2
    }
}
