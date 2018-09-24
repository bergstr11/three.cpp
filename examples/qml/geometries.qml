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
        shadows.type: Three.PCFSoft

        Scene {
            id: scene
            bgColor: Qt.rgba(0.4,0.4,0.6)

            Axes {
                size: 20
            }

            Plane {
                width: 60
                height: 20

                position: "15,0,0"
                rotation.x: -0.5 * Math.PI

                material: MeshPhongMaterial {
                    color: "#919191"
                    dithering: true
                }
                receiveShadow: true
            }

            Box {
                id: box
                width: 4
                height: 4
                depth: 4

                position: "-4,3,0"

                material: MeshLambertMaterial {
                    color: "#ff0000"
                }
                castShadow: true
            }

            Torus {
                id: torus
                radius: 4
                tube: 0.7
                radialSegments: 8
                tubularSegments: 24

                position: "5,8,2"
                rotation: "2,4,6"

                material: MeshLambertMaterial {
                    color: "#36aa22"
                }
                castShadow: true
            }

            Sphere {
                id: sphere
                radius: 4
                widthSegments: 20
                heightSegments: 20

                position: "25,4,2"

                material: MeshLambertMaterial {
                    color: "#7777ff"
                }
                castShadow: true
            }

            AmbientLight {
                color: "#ffffff"
                intensity: 0.35
            }

            SpotLight {
                color: "#ffffff"
                position: "-40,60,-10"
                castShadow: true

                shadow.mapSize: "2048x2048"
            }

            camera: PerspectiveCamera {
                fov: 45
                aspect: threeD.width / threeD.height
                near: 0.1
                far: 1000

                position: "-30,40,30"

                target: scene.position

                controller: OrbitController {
                    enablePan: false
                }
            }
        }
    }
}
