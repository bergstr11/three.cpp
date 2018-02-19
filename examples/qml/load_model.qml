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
        id: lrow
        anchors.top: parent.top
        anchors.topMargin: 10
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: 20
        z: 2

        Label {id: dlabel; text: "Distance: %1".arg(orbitController.distance)}
        Label {id: rlabel; text: "Rotation: %1:%2:%3".arg(modelref.rotation.x).arg(modelref.rotation.y).arg(modelref.rotation.z)}

    }
    MouseArea {
        anchors.fill: lrow
        z: 3
        onClicked: {
            console.log(dlabel.text, rlabel.text)
        }
    }
    OptionsMenu {
        id: lightControls
        title: "light"
        anchors.top: parent.top
        anchors.left: parent.left
        width: 350
        color: "transparent"
        z: 2
        threeD: threeD

        FloatValue {
            name: "intensity"
            target: hemisphereLight
            from: 0
            to: 2
        }
    }
    OptionsMenu {
        id: objectControls
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
        BoolValue {
            name: "enablePan"
            target: orbitController
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
        samples: 24

        Model {
            id: threeDModel
            onFileChanged: holdon.visible = true
            onModelLoaded: holdon.visible = false
        }

        Scene {
            id: scene

            HemisphereLight {
                id: hemisphereLight
                skyColor: "#ababab"
                intensity: 0.4
            }

            ModelRef {
                id: modelref
                model: threeDModel
                type: ModelRef.Node
                replace: true

                onObjectChanged: {
                    orbitController.reset()
                    objectControls.reset()
                    lightControls.reset()
                }
            }

            camera: PerspectiveCamera {
                id: sceneCamera
                aspect: threeD.width / threeD.height
                near: 1
                far: 100000

                position: "0,0,1000"

                lookAt: scene.position

                controller: OrbitController {
                    id: orbitController
                    maxPolarAngle: Math.PI;
                    enablePan: false
                }

                PointLight {
                    color: "#ffffff"
                    intensity: 0.5
                    position: Qt.vector3d(12,15,1).times(30)
                }
                PointLight {
                    color: "#ffffff"
                    intensity: 0.75
                    position: Qt.vector3d(-12, 17.6, 2.4).times(30)
                }
                /*DirectionalLight {
                    color: "#cccccc"
                    position: "0,0,400"
                    intensity: 0.2
                }*/
            }
        }
    }
}
