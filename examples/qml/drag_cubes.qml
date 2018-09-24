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
        anchors.right: parent.right
        color: "transparent"
        width: 350
        height: 200
        z: 2

        MenuChoice {
            name: "Boxes"
            textColor: "black"
            value: true
            onSelected: {
                sphere.visible = false
                spherePicker.enabled = false
                randomCubes.visible = true
			    dragger.objects = randomCubes.objects
                dragger.surface = []
                threeD.update()
            }
        }
        MenuChoice {
            name: "Sphere"
            textColor: "black"
            onSelected: {
                sphere.visible = true
                spherePicker.enabled = true
                randomCubes.visible = false
                dragger.objects = sphere.objects
                dragger.surface = [sphere]
                threeD.update()
            }
        }
    }
    Component {
        id: boxFactory
        Box {
            width: 50
            height: 50
            depth: 50

            material: MeshLambertMaterial {
                id: boxMaterial
                color: Three.randomColor(0xffffff)
            }
        }
    }

    ThreeD {
        id: threeD
        anchors.fill: parent
        focus: true
        shadows.type: Three.PCFSoft

        ObjectDragger {
            id: dragger
            camera: scene.camera
        }

        ObjectPicker
        {
            id: spherePicker
            enabled: false
            camera: scene.camera
            objects: [sphere]

            onObjectsClicked: {
                var is = intersect(0)
                sphere.createObject(is.point, is.faceNormal)

                enabled = false
            }
        }
        Scene {
            id: scene
            bgColor: "#f0f0f0"

            AmbientLight {
                color: "#505050"
            }
            SpotLight {
                color: "#ffffff"
                intensity: 1.5
                position: "0,500,2000"
				angle: Math.PI / 9
				castShadow: true
				shadow.camera.near: 1000
				shadow.camera.far: 4000
				shadow.mapSize:"1024x1024"
            }
            Node {
                id: randomCubes
                visible: true

                readonly property var objects: new Array()

                Component.onCompleted: {
                    for ( var i = 0; i < 100; i ++ ) {
                        var box = boxFactory.createObject(randomCubes)
                        box.scale.x = Math.random() * 2 + 1;
                        box.scale.y = Math.random() * 2 + 1;
                        box.scale.z = Math.random() * 2 + 1;
                        box.position.x = Math.random() * 1000 - 500;
                        box.position.y = Math.random() * 600 - 300;
                        box.position.z = Math.random() * 800 - 400;
                        box.rotation.x = Math.random() * 2 * Math.PI;
                        box.rotation.y = Math.random() * 2 * Math.PI;
                        box.rotation.z = Math.random() * 2 * Math.PI;
                        box.castShadow = true
                        box.receiveShadow = true

                        randomCubes.add( box );
                        objects.push(box)
                    }
                    dragger.objects = objects
                }
            }
            Sphere {
                id: sphere
                radius: 400
                widthSegments: 48
                heightSegments: 24
                visible: false
                name: "sphere"

                material: MeshLambertMaterial {
                    envMap: ImageTexture {
                                id: textureSphere
                                format: Texture.RGBA
                                mapping: Texture.SphericalReflection
                                image: ":/metal.jpg"
                                imageFormat: ThreeImage.RGBA8888
                            }
                }
                readonly property var objects: new Array()

                function createObject(point, faceNormal)
                {
                    var box = boxFactory.createObject(sphere)
                    box.scale.z = 0.5
                    box.position = point
                    box.castShadow = true
                    box.receiveShadow = true
                    box.name = "box"

                    sphere.add( box );

                    box.lookAt(sphere.position)

                    objects.push(box)
                    dragger.objects = objects
                }
            }

            camera: PerspectiveCamera {
                fov: 70
                aspect: threeD.width / threeD.height

                position: "0,0,1000"

                target: scene.position
            }
        }
    }
}
