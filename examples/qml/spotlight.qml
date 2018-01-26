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
        title: "spotlight"
        anchors.top: parent.top
        anchors.right: parent.right
        width: 350
        color: "transparent"
        z: 2
        threeD: threeD

        FloatValue {
            name: "intensity"
            target: spotLight
            from: 0
            to: 2
        }
        FloatValue {
            name: "distance"
            target: spotLight
            from: 50
            to: 200
        }
        FloatValue {
            name: "angle"
            target: spotLight
            from: 0
            to: Math.PI / 3
        }
        FloatValue {
            name: "penumbra"
            target: spotLight
            from: 0
            to: 1
        }
        FloatValue {
            name: "decay"
            target: spotLight
            from: 1
            to: 2
        }
    }
    OptionsMenu {
        title: "ambientlight"
        anchors.top: parent.top
        anchors.left: parent.left
        width: 350
        color: "transparent"
        z: 2
        threeD: threeD

        FloatValue {
            name: "intensity"
            target: ambientLight
            from: 0
            to: 2
        }
    }

    ThreeD {
        id: threeD
        anchors.fill: parent
        focus: true
        z: 1
        shadowType: Three.PCFSoft

        Scene {
            id: scene
            background: Qt.rgba(0.4,0.4,0.6)

            AmbientLight {
                id: ambientLight
                color: "#ffffff"
                intensity: 0.1
            }

            SpotLight {
                id: spotLight
                color: "#ffffff"
                position: "15,40,35"
                intensity: 1
                distance: 200
                angle: Math.PI / 4
                penumbra: 0.05
                decay: 2
                castShadow: true

                shadow.mapSize: "1024x1024"
                shadow.camera.near: 0.1
                shadow.camera.far: 200

                addHelper: true
            }

            Axes {
                size: 10
            }

            Box {
                name: "plane"
                width: 2000
                height: 1
                depth: 2000

                position: "0,-1,0"

                material: MeshPhongMaterial {
                    color: "#808080"
                    dithering: true
                }
                receiveShadow: true
            }

            Box {
                width: 3
                height: 1
                depth: 2

                position: "40,2,0"

                material: MeshPhongMaterial {
                    color: "#4080ff"
                    dithering: true
                }
                castShadow: true
            }

            camera: PerspectiveCamera {
                fov: 35
                aspect: scene.width / scene.height
                near: 1
                far: 1000

                position: "65,8,-10"

                lookAt: scene.position

                controller: OrbitController {
                    minDistance: 20
                    maxDistance: 500
                    enablePan: false
                }
            }
        }
    }
}
