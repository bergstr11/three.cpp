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

    Label {
        id: holdon
        anchors.centerIn: parent
        text: "Stand by, we're loading.."
        font.pointSize: 34
        visible: false
    }

    ThreeD {
        id: threeD
        anchors.fill: parent
        autoClear: false

        Model {
            id: threeDModel
            file: ":/portalgun.3ds"
            onFileChanged: holdon.visible = true
            onModelLoaded: holdon.visible = false
            replacements: {"color.jpg": ":/portalgun_color.jpg"}
        }

        ImageTexture {
            id: normalTexture
            format: Texture.RGBA
            image: ":/portalgun_normal.jpg"
            imageFormat: Image.RGBA8888
        }

        Scene {
            id: scene

            HemisphereLight {
            }

            DirectionalLight {
                color: "#ffeedd"
                position: "0,0,2"
            }

            ModelRef {
                model: threeDModel
                type: ModelRef.Mesh
                selector: "*:PortalGun"

                onObjectChanged: {
                    object.material.normalMap = normalTexture
                }
            }

            camera: PerspectiveCamera {
                id: sceneCamera
                fov: 70
                aspect: threeD.width / threeD.height
                near: 1
                far: 100000

                position: "0,0,2"

                lookAt: scene.position

                controller: OrbitController {
                    minDistance: 2
                    maxDistance: 500
                    enablePan: true
                }
            }
        }
    }
}
