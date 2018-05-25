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

        ImageCubeTexture {
            id: textureCube
            format: Texture.RGBA
            mapping: Texture.CubeReflection
            flipY: false
            type: Texture.UnsignedByte
            images: [
                ThreeImage {url: ":/pontiac/pisa/px.png"; format: ThreeImage.RGBA8888},
                ThreeImage {url: ":/pontiac/pisa/nx.png"; format: ThreeImage.RGBA8888},
                ThreeImage {url: ":/pontiac/pisa/py.png"; format: ThreeImage.RGBA8888},
                ThreeImage {url: ":/pontiac/pisa/ny.png"; format: ThreeImage.RGBA8888},
                ThreeImage {url: ":/pontiac/pisa/pz.png"; format: ThreeImage.RGBA8888},
                ThreeImage {url: ":/pontiac/pisa/nz.png"; format: ThreeImage.RGBA8888}
            ]
        }

        Model {
            id: threeDModel
            onFileChanged: holdon.visible = true
            onModelLoaded: holdon.visible = false

            file: ":/pontiac/Pontiac-Lemans-1965.obj"
            resourcePrefix: ":/pontiac/"

            MaterialHandler {
                materialNames: "chrome,Chrome1,chrome2,light_chrome"
                createAs: MeshStandardMaterial {
                    color: "white"
                    metalness: 1.0
                    roughness: 1
                    envMap: textureCube
                }
            }
            MaterialHandler {
                materialNames: "Material__668,Material__681"
                createAs: MeshStandardMaterial {
                    color: "#C14200"
                    roughness: 0
                    envMap: textureCube
                }
            }
            MaterialHandler {
                materialNames: "Material__756"
                createAs: MeshStandardMaterial {
                    roughness: 0
                    envMap: textureCube
                }
            }
        }

        Scene {
            id: scene
            bgTexture: textureCube

            PointLight {
                id: light1
                color: "#ffffff"
                position: Qt.vector3d(2, 5, 1).times(30)
            }
            PointLight {
                id: light2
                color: "#ffffff"
                intensity: 0.75
                position: Qt.vector3d(-12, 4.6, 2.4).times(30)
            }
            AmbientLight {
                id: light3
                color: "#050505"
            }

            ModelRef {
                id: modelref
                model: threeDModel
                type: ModelRef.Node
                replace: true

                onObjectChanged: {
                    orbitController.reset()

                    object.position = "0,-25,20"
                    object.rotation = Qt.vector3d(1.60,3.2,3.35)
                    object.scale = Qt.vector3d(1.9, 1.9, 1.9)
                }
            }

            camera: PerspectiveCamera {
                id: sceneCamera
                fov: 50
                aspect: threeD.width / threeD.height
                near: 1
                far: 100000

                position: "185,40,170"

                target: scene.position

                controller: OrbitController {
                    id: orbitController
                    maxPolarAngle: Math.PI / 2;
                    minDistance: 150
                    maxDistance: 500
                    rotateCursor: Qt.ClosedHandCursor
                    panCursor: Qt.SizeAllCursor
                }
            }
        }
    }
}
