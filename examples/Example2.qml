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

    Rectangle {
        anchors.top: mainWindow.top
        anchors.right: mainWindow.right
        Layout.margins: 10
        width: 350
        height: 200
        color: darkgray
        z: 2
        
        Slider {
            id: intensity
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            from: 0
            to: 2
            value: spotLight.intensity

            onValueChanged: {
                spotLight.intensity = value
                scene.update()
            }
        }
        Slider {
            id: distance
            anchors.top: intensity.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            from: 50
            to: 200
            value: spotLight.distance

            onValueChanged: {
                spotLight.distance = value
                scene.update()
            }
        }
        Slider {
            id: angle
            anchors.top: distance.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            from: 0
            to: Math.PI / 3
            value: spotLight.angle

            onValueChanged: {
                spotLight.angle = value
                scene.update()
            }
        }
        Slider {
            id: penumbra
            anchors.top: angle.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            from: 0
            to: 1
            value: spotLight.penumbra

            onValueChanged: {
                spotLight.penumbra = value
                scene.update()
            }
        }
        Slider {
            id: decay
            anchors.top: penumbra.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            from: 1
            to: 2
            value: spotLight.decay

            onValueChanged: {
                spotLight.decay = value
                scene.update()
            }
        }
    }

    Scene {
        id: scene
        z: 1
        anchors.fill: parent
        focus: true
        background: Qt.rgba(0.4,0.4,0.6)
        shadowType: Scene.PCFSoft

        AmbientLight {
            id: ambientLight
            color: "#ffffff"
            intensity: 0.4
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
            shadow.camera.near: 10
            shadow.camera.far: 200
        }

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
