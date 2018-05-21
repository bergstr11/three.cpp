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

        /*MeshStandardMaterial {
            id: body
            color: "#000000"
            lightMap: ?
            envMap: textureCube
            metalness: 0.5
            roughness: 0.3
        }
        MeshStandardMaterial {
            id: tire_car
            color: "#000000"
            lightMap: ?
            metalness: 0.1
            roughness: 0.9
        }
        MeshStandardMaterial {
            id: mirror
            color: "#000000"
            lightMap: ?
            envMap: textureCube
            metalness: 0.9
            roughness: 0.5
        }
        MeshStandardMaterial {
            id: glass
            color: ?
            envMap: textureCube
            metalness: 1
            roughness: 0.1
            opacity: ?
        }
        MeshPhongMaterial {
            id: mat01
            color: "#404040"
            map: ?
            specularMap: ?
            shininess: 30
            metl: true
        }*/

        /*Model {
            id: threeDModel
            onFileChanged: holdon.visible = true
            onModelLoaded: holdon.visible = false

            file: ":/pontiac/Pontiac-Lemans-1965.obj"
            resourcePrefix: ":/pontiac/"
        }*/

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

            Box {
                id: box
                width: 10
                height: 10
                depth: 10

                position: "-105,-78,-30"
                scale: "30,30,30"

                material: MeshLambertMaterial {
                    color: "#ff0000"
                }
            }

            /*ModelRef {
                id: modelref
                model: threeDModel
                type: ModelRef.Node
                replace: true

                onObjectChanged: {
                    orbitController.reset()

                    object.position = "0,-20,0"
                    object.rotation = Qt.vector3d(1.55,3.2,2.55)
                    object.scale = Qt.vector3d(1.2, 1.2, 1.2)
                }
            }*/

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
