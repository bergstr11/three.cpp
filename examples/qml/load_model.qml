import QtQuick 2.7
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.1
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.0

import three.quick 1.0

Window {
    id: mainWindow
    minimumWidth: 1280
    minimumHeight: 1024
    visible: true

    Rectangle {
        anchors.fill: parent
        color: "lightgray"
    }

    FileDialog {
        id: fileDialog
        title: "Please choose a model file"
        folder: shortcuts.home
        selectMultiple: false
        selectExisting: true
        onAccepted: {
            threeDModel.file = fileDialog.fileUrl
        }
    }
    Button {
        text: "Choose.."
        z: 2
        onClicked: fileDialog.visible = true
    }

    Label {
        id: holdon
        anchors.centerIn: parent
        text: "Stand by, we're loading.."
        font.pointSize: 34
        visible: false

        RotationAnimation on rotation {
            loops: Animation.Infinite
            from: 0
            to: 360
            duration: 4000
            running: holdon.visible
        }
    }

    ThreeD {
        id: threeD
        anchors.fill: parent
        autoClear: false
        samples: 8

        Model {
            id: threeDModel
            isScene: false
            onFileChanged: holdon.visible = true
            onModelLoaded: holdon.visible = false
        }

        Scene {
            id: scene

            DirectionalLight {
                color: "#ffeedd"
                position: "0,0,2"
            }

            AmbientLight {
                id: ambientLight
                color: "#cccccc"
            }

            ModelRef {
                model: threeDModel
                type: ModelRef.Node
            }

            camera: PerspectiveCamera {
                id: sceneCamera
                fov: 70
                aspect: threeD.width / threeD.height
                near: 1
                far: 100000

                position: "0,0,1000"

                lookAt: scene.position

                controller: OrbitController {
                    minDistance: 500
                    maxDistance: 2500
                    enablePan: false
                }
            }
        }
    }
}
