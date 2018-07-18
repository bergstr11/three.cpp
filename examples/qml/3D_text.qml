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
        threeD.update()
    }

    OptionsMenu {
        anchors.top: parent.top
        anchors.left: parent.left
        color: "transparent"
        width: 250
        height: 100
        z: 2

        MenuChoice {
            name: "Optimer"
            value: true
            textColor: "white"
            onSelected: {
                setFont("optimer")
            }
        }
        MenuChoice {
            name: "Gentilis"
            textColor: "white"
            onSelected: {
                setFont("gentilis")
            }
        }
        MenuChoice {
            name: "Helvetiker"
            textColor: "white"
            onSelected: {
                setFont("helvetiker")
            }
        }
        BoolChoice {
            name: "Bold"
            value: false
            onValueChanged: {
                fontWeight = value ? "bold" : "regular"
                setFont()
            }
        }
        BoolChoice {
            name: "Bevel"
            value: true
            onValueChanged: {
                text3d.bevelEnabled = value
                threeD.update()
            }
        }
    }

    ThreeD {
        id: threeD
        anchors.fill: parent
        focus: true
        shadowType: Three.PCFSoft

        Scene {
            id: scene
            bgColor: "#000000"
            fog: Fog {color: "#000000"; near: 250; far: 1400}

            DirectionalLight {
                color: "#ffffff"
                intensity: 0.25
                position: Qt.vector3d( 0, 0, 1 ).normalized();
            }

            Plane {
                name: "plane"
                type: Three.BufferGeometry
				width: 10000
				height: 10000
				position: "0,100,0"
				rotation: Qt.vector3d(- Math.PI / 2, 0, 0)

				material: MeshBasicMaterial {
				    color: "#ffffff"
				    opacity: 0.5
				}
			}

            Node {
                id: textsNode
                position: "0,100,0"

                Text3D {
                    id: text3d
                    name: "text3d"
                    text: "Hello Three::pp"
                    type: Three.BufferGeometry
                    size: 70
                    height: 20
                    curveSegments: 4
                    bevelThickness: 2
                    bevelSize: 1.5
                    bevelEnabled: true
                    castShadow: true
                    receiveShadow: true

                    rotation: Qt.vector3d(0, Math.PI * 2,0)

                    materials: [
                      MeshPhongMaterial {color: "white"; flatShading: true},
                      MeshPhongMaterial {color: "white"; flatShading: true}
                    ]
                }

                Text3D {
                    id: textMirror
                    name: "textMirror"
                    copy: text3d

                    rotation: Qt.vector3d(Math.PI, Math.PI * 2, 0)
                }

                Component.onCompleted: {
                    setFont()

                    var centerOffset = -0.5 * ( text3d.boundingBox.max.x - text3d.boundingBox.min.x );

                    text3d.position = Qt.vector3d(centerOffset, 30, 0)
                    textMirror.position = Qt.vector3d(centerOffset, -30, 20)
                }
            }

            camera: PerspectiveCamera {
                fov: 30
                aspect: threeD.width / threeD.height
                near: 1
                far: 1500

                position: "0,400,950"

                target: "0,150,0"

                controller: OrbitController {
                    enablePan: false
                }

                PointLight {
                    color: Qt.hsla(1, 1, 0.5, 1)
                    intensity: 1.5
                    position: "30,100,800";
                }
            }
        }
    }
}
