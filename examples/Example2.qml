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
        shadowType: Scene.PCFSoft

        Axes {
            size: 10
        }

        Box {
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

        AmbientLight {
            color: "#ffffff"
            intensity: 0.1
        }

        SpotLight {
            color: "#ffffff"
            position: "-40,60,-10"
            intensity: 1
            distance: 200
            angle: Math.PI / 4
            penumbra: 0.05
            decay: 2
            castShadow: true

            shadow.mapSize: "1024x1024"
        }

        camera: PerspectiveCamera {
            fov: 35
            aspect: scene.width / scene.height
            near: 1
            far: 1000

            position: "65,8,-10"

            lookAt: scene
        }
    }
}
