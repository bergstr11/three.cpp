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

    OptionsMenu {
        anchors.top: parent.top
        anchors.left: parent.left
        color: "transparent"
        width: 250
        height: 100
        z: 2

        BoolChoice {
            name: "VertexNormals"
            value: false
            onValueChanged: {
                box.vertexNormals.visible = value
                torus.vertexNormals.visible = value
                sphere.vertexNormals.visible = value
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
                intensity: 0.125
				position: Qt.vector3d(0,0,1).normalized()
            }

			PointLight {
			    color: "#f0f0f0"
			    intensity: 1.5
				position: "0,100,90";
			}

            Plane {
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

            Text3D {
                id: text3d
                text: "Hello Three::pp"
                type: Three.BufferGeometry
				font: ":/optimer_bold.typeface.json"
				size: 70
				height: 20
				curveSegments: 4
				bevelThickness: 2
				bevelSize: 1.5
				bevelEnabled: true

				rotation: Qt.vector3d(0,Math.PI * 2,0)

                materials: [
                    MeshPhongMaterial{
                        id: frontMaterial
                        color: "#ffffff";
                        flatShading: true
                    },

                    MeshPhongMaterial {
                        id: sideMaterial
                        color: "#ffffff"
                    }
                ]
				Component.onCompleted: {
                    var centerOffset = -0.5 * ( text3d.boundingBox.max.x - text3d.boundingBox.min.x );
				    text3d.position = Qt.vector3d(centerOffset, 30, 0)
				}
            }

            camera: PerspectiveCamera {
                fov: 30
                aspect: threeD.width / threeD.height
                near: 1
                far: 1500

                position: "0,400,700"

                target: "0,150,0"

                controller: OrbitController {
                    enablePan: false
                }
            }
        }
    }
}
