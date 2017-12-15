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

    Scene {
        id: scene
        z: 1
        anchors.fill: parent
        focus: true
        background: Qt.rgba(0.4,0.4,0.6)

        Axes {
            size: 20
        }

        Plane {
            width: 60
            height: 20

            position: "15,0,0"
            rotation.x: -0.5 * Math.PI

            material: MeshBasicMaterial {
                color: "#cccccc"
            }
        }

        Box {
            width: 4
            height: 4
            depth: 4

            position: "-4,3,0"

            material: MeshBasicMaterial {
                color: "#ff0000"
                wireframe: true
            }
        }

        Sphere {
            radius: 4
            widthSegments: 20
            heightSegments: 20

            position: "20,4,2"

            material: MeshBasicMaterial {
                color: "#7777ff"
                wireframe: true
            }
        }

        camera: PerspectiveCamera {
            fov: 45
            aspect: scene.width / scene.height
            near: 0.1
            far: 1000

            position: "-30,40,30"

            lookAt: scene
        }
    }
}
