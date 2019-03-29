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

    Component {
        id: lightFactory

        PointLight {
            id: pointLight
            distance: 20
            intensity: 1.5
            castShadow: true
            shadow.camera.near: 1
            shadow.camera.far: 60
            shadow.bias: -0.005

            Sphere {
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

                    alphaMap: ColorTexture {
                        color: "white"
                        width: 2
                        height: 2
                        magFilter: Three.NearestFilter
                        wrapT: Three.RepeatWrapping
                        wrapS: Three.RepeatWrapping
                        repeat: "1,3.5"
                    }
                }
                customDistanceMaterial: MeshDistanceMaterial {
                    alphaMap: sphere2.material.alphaMap
                    alphaTest: sphere2.material.alphaTest
                }
            }
        }
    }
    ThreeD {
        id: threeD
        anchors.fill: parent
        focus: true
        shadowMap.type: Three.BasicShadow
        autoAnimate: false

        property var pointLight
        property var pointLight2

        Scene {
            id: scene

            AmbientLight {
                color: "#111122"
            }

            Box {
                id: box1
                type: Three.BufferGeometry
                width: 30
                height: 30
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
            }

            Component.onCompleted: {
                threeD.pointLight = lightFactory.createObject(scene, {color: "#0088ff"})
                threeD.pointLight2 = lightFactory.createObject(scene, {color: "#ff8888"})
            }
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
