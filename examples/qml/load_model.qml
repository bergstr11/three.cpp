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
    Row {
        anchors.top: parent.top
        anchors.topMargin: 10
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: 20

        Label {text: "Position: %1:%2:%3".arg(modelref.position.x).arg(modelref.position.y).arg(modelref.position.z)}
        Label {text: "Rotation: %1:%2:%3".arg(modelref.rotation.x).arg(modelref.rotation.y).arg(modelref.rotation.z)}
    }
    OptionsMenu {
        title: "adjust model"
        anchors.top: parent.top
        anchors.right: parent.right
        width: 350
        color: "transparent"
        z: 2
        threeD: threeD

        FloatManip {
            name: "rotation.x"
            target: modelref
            from: -Math.PI
            to: Math.PI
            onValueChanged: {
                target.rotateX(value - base)
                base = value
            }
        }
        FloatManip {
            name: "rotation.y"
            target: modelref
            from: -Math.PI
            to: Math.PI
            onValueChanged: {
                target.rotateY(value - base)
                base = value
            }
        }
        FloatManip {
            name: "rotation.z"
            target: modelref
            from: -Math.PI
            to: Math.PI
            onValueChanged: {
                target.rotateZ(value - base)
                base = value
            }
        }
        FloatManip {
            name: "position.z"
            target: sceneCamera
            from: -10000
            to: 10000
            onValueChanged: {
                target.translateZ(value - base)
                base = value
            }
        }
        BoolChoice {
            name: "Enable pan"
            value: controller.enablePan
            onValueChanged: {
                controller.enablePan = value
            }
        }
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
        samples: 12

        Model {
            id: threeDModel
            isScene: false
            onFileChanged: holdon.visible = true
            onModelLoaded: holdon.visible = false
        }

        Scene {
            id: scene

            AmbientLight {
                id: ambientLight
                color: "#cccccc"
            }

            ModelRef {
                id: modelref
                model: threeDModel
                type: ModelRef.Node
            }

            camera: PerspectiveCamera {
                id: sceneCamera
                aspect: threeD.width / threeD.height
                near: 1
                far: 2000

                position: "0,0,1000"

                lookAt: scene.position

                controller: OrbitController {
                    id: controller
                    minDistance: 150
                    maxDistance: 1500
                    maxPolarAngle: Math.PI;
                    enablePan: false
                }

                DirectionalLight {
                    color: "#dddddd"
                    position: "0,0,2"
                    intensity: 0.5
                }
            }
        }
    }
}
