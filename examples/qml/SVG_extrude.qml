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

    property string fontName: "optimer"
    property string fontWeight: "regular"

    function setFont(fname) {
        if(fname) fontName = fname
        text3d.font = ":/" + fontName + "_" + fontWeight + ".typeface.json"

        textMirror.copy(text3d)

        threeD.update()
    }

    OptionsMenu {
        anchors.top: parent.top
        anchors.left: parent.left
        color: "transparent"
        width: 250
        height: 100
        z: 2

        BoolChoice {
            name: "Bevel"
            value: true
            onValueChanged: {
                lo3d.bevelEnabled = value
                threeD.update()
            }
        }
    }

    ThreeD {
        id: threeD
        anchors.fill: parent
        focus: true
        shadowMap.type: Three.PCFSoftShadow

        Scene {
            id: scene
            bgColor: "lightgray"

            AmbientLight {

            }
            DirectionalLight {
                color: "#ffffff"
                intensity: 0.25
                position: Qt.vector3d( 0, 0, 1 ).normalized();
            }

            SVG {
                id: lo3d
                file: ":/LO3D.svg"
                depth: 20
                bevelThickness: 0.5
                bevelSize: 1.5
                bevelEnabled: true
                useSvgColors: true
                useShapeGroups: true
                castShadow: true
                receiveShadow: true

                rotation: Qt.vector3d(Math.PI,0,0)
                position: "50,150,0"

                materials: [
                  MeshPhongMaterial {flatShading: true},
                  MeshPhongMaterial {flatShading: true},
                  MeshPhongMaterial {flatShading: true},
                  MeshPhongMaterial {flatShading: true}
                ]
            }

            camera: PerspectiveCamera {
                fov: 30
                aspect: threeD.width / threeD.height
                near: 1
                far: 1500

                position: "0,400,950"

                target: "50,150,0"
            }
        }
        animate: function() {
            lo3d.rotateY(0.005)
        }
    }
}
