import QtQuick 2.7
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.1
import QtQuick.Window 2.2

import com.lightningobjects.ui 1.0

Window {
    id: mainWindow
    minimumWidth: 1024
    minimumHeight: 768

    visible: true

    /*Text {
        anchors.centerIn: parent
        font.pointSize: 50
        text: "Hello World"
        visible: false
        color: "white"
        z: 2
    }*/
    ThreeDItem {
        anchors.fill: parent
        focus: true
        background: "yellow"
        data: "/home/byter/work/git/vtk_qml/data/Mercedes_GLE6/gle63amg_lowpoly.3DS"
    }
}
