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

    ColorDialog {
        id: colorDialog
        title: "Please choose a color"
        visible: false

        property Light light
        property string colorName: "color"

        onVisibleChanged: {
            if(visible) {
                color = light[colorName]
            }
        }
        onAccepted: {
            if(light) {
                light[colorName] = colorDialog.color
                threeD.update()
            }
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
            label: "hemisphere"
            name: "intensity"
            target: hemisphereLight
            from: 0
            to: 2
        }
        FloatValue {
            label: "directional"
            name: "intensity"
            target: directionalLight
            from: 0
            to: 2
        }
        MenuChoice {
            name: "hemisphere sky color"
            isAction: true
            onSelected: {
                colorDialog.light = hemisphereLight
                colorDialog.colorName = "skyColor"
                colorDialog.visible = true
            }
        }
        MenuChoice {
            name: "hemisphere ground color"
            isAction: true
            onSelected: {
                colorDialog.light = hemisphereLight
                colorDialog.colorName = "groundColor"
                colorDialog.visible = true
            }
        }
        MenuChoice {
            name: "directional color"
            isAction: true
            onSelected: {
                colorDialog.light = directionalLight
                colorDialog.colorName = "color"
                colorDialog.visible = true
            }
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
        BoolChoice {
            name: "Convex Hull"
            value: convexHull.visible
            onValueChanged: {
                convexHull.visible = value
                threeD.update()
            }
        }
        BoolChoice {
            name: "Prefer Phong"
            value: threeDModel.options.preferPhong
            onValueChanged: {
                threeDModel.options.preferPhong = value
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
                skyColor: Qt.hsla(0.6, 1, 0.6, 1)
                groundColor: Qt.hsla(0.095, 1, 0.75, 1)
                intensity: 0.6
                position.y: 250
            }

            ConvexHull {
                id: convexHull
                visible: false

                material: MeshBasicMaterial {color: "green"; wireframe: true}

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
            }

            camera: PerspectiveCamera {
                id: sceneCamera
                aspect: threeD.width / threeD.height
                near: 1
                far: 100000

                position: "0,0,1000"

                target: scene.position

                controller: OrbitController {
                    id: orbitController
                    maxPolarAngle: Math.PI;
                    enablePan: true
                    rotateCursor: Qt.ClosedHandCursor
                    panCursor: Qt.SizeAllCursor
                }

                DirectionalLight {
                    id: directionalLight
                    color: Qt.hsla(0.1, 1, 0.95, 1)
                    position: Qt.vector3d(-1.3,1.75,1).times(130)
                    intensity: 0.5
                }
            }
        }
    }
}
