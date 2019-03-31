import QtQuick 2.7
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.1
import QtQuick.Window 2.2

import three.quick 1.0

Window {
    id: mainWindow
    minimumWidth: 1280
    minimumHeight: 1024
    color: "black"
    visible: true

    OptionsMenu {
        anchors.top: parent.top
        anchors.left: parent.left
        color: "transparent"
        width: 350
        height: 100
        z: 2

        BoolChoice {
            label: "shadows:"
            value: false
            onValueChanged: {
                threeD.shadowMap.type = value ? Three.BasicShadow : Three.NoShadow
                threeD.pointLight.updateMaterials()
                threeD.pointLight2.updateMaterials()
                box1.updateMaterials()
                threeD.update()
            }
        }
        BoolChoice {
            name: "animate:"
            value: false
            onValueChanged: {
                threeD.runAnimation(value)
            }
        }
    }

    Component {
        id: lightFactory

        PointLight {
            id: pointLight
            intensity: 1.5
            decay: 1
            distance: 20
            castShadow: true
            shadow.camera.near: 1
            shadow.camera.far: 60
            shadow.bias: -0.005

            Sphere {
                id: sphere1
                type: Three.BufferGeometry
                radius: 0.3
                widthSegments: 12
                heightSegments: 6

                material: MeshBasicMaterial {
                    color: Qt.lighter(pointLight.color, pointLight.intensity)
                }
            }
            Sphere {
                id: sphere2
                type: Three.BufferGeometry
                radius: 2
                widthSegments: 32
                heightSegments: 8
                castShadow: true
                receiveShadow: true

                material: MeshPhongMaterial {
                    side: Three.DoubleSide
                    alphaTest: 0.5

                    map: ColorTexture {
                        color: "transparent"
                        fillColor: "white"
                        width: 2
                        height: 2
                        fillRect: Qt.rect(0,1,2,1)
                        magFilter: Three.NearestFilter
                        wrapT: Three.RepeatWrapping
                        wrapS: Three.RepeatWrapping
                        repeat: "1,3.5"
                    }
                }
                customDistanceMaterial: MeshDistanceMaterial {
                    alphaMap: sphere2.material.map
                    alphaTest: sphere2.material.alphaTest
                }
            }
        }
    }
    ThreeD {
        id: threeD
        anchors.fill: parent
        focus: true
        shadowMap.type: Three.NoShadow
        antialias: true
        autoAnimate: false

        property var pointLight
        property var pointLight2

        /*ShadowMapViewer {
            scale: 0.4
            position: "700,10"
            light: threeD.pointLight
            enabled: !!threeD.pointLight && threeD.pointLight.visible
        }*/

        Scene {
            id: scene

            AmbientLight {
                color: "#111122"
            }

            Box {
                id: box1
                type: Three.BufferGeometry
                width: 30
                height: 37
                depth: 30
                receiveShadow: true
                position.y: 10

                material: MeshPhongMaterial {
                    color: "#a0adaf"
                    shininess: 10
                    specular: "#111111"
                    side: Three.BackSide
                }
            }

            camera: PerspectiveCamera {
                fov: 45
                aspect: threeD.width / threeD.height
                near: 1
                far: 1000
                position: "0,10,40"

                target: scene.position
                controller: OrbitController {}
            }

        }
        Component.onCompleted: {
            pointLight = lightFactory.createObject(scene, {color: "#0088ff"})
            scene.add(pointLight)
            pointLight2 = lightFactory.createObject(scene, {color: "#ff8888"})
            scene.add(pointLight2)
        }
        animate: function() {
            var time = Date.now() * 0.001;

            pointLight.position.x = Math.sin( time * 0.6 ) * 9;
            pointLight.position.y = Math.sin( time * 0.7 ) * 9 + 5;
            pointLight.position.z = Math.sin( time * 0.8 ) * 9;

            pointLight.rotation.x = time;
            pointLight.rotation.z = time;

            time += 10000;

            pointLight2.position.x = Math.sin( time * 0.6 ) * 9;
            pointLight2.position.y = Math.sin( time * 0.7 ) * 9 + 5;
            pointLight2.position.z = Math.sin( time * 0.8 ) * 9;

            pointLight2.rotation.x = time;
            pointLight2.rotation.z = time;
        }
    }
}
