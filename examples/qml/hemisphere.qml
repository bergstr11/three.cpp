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
        anchors.left: parent.left
        color: "transparent"
        width: 250
        height: 200
        z: 2

        BoolChoice {
            name: "DirectionalLight"
            value: dirlight.visible
            onValueChanged: {
                dirlight.visible = value
                dirlight.helper.visible = value
                threeD.update()
            }
        }
        BoolChoice {
            name: "HemisphereLight"
            value: hemilight.visible
            onValueChanged: {
                hemilight.visible = value
                hemilight.helper.visible = value
                threeD.update()
            }
        }
    }

    ThreeD {
        id: threeD
        anchors.fill: parent
        focus: true
        shadowMap.type: Three.PCFSoftShadow

        Model {
            id: eagleModel
            file: ":/eagle.3ds"
        }

        ShaderMaterial {
            id: skyMaterial
            side: Three.BackSide
            vertexShader:
                "out vec3 vWorldPosition;
                void main() {
                    vec4 worldPosition = modelMatrix * vec4( position, 1.0 );
                    vWorldPosition = worldPosition.xyz;
                    gl_Position = projectionMatrix * modelViewMatrix * vec4( position, 1.0 );
                }"
            fragmentShader:
                "uniform vec3 topColor;
                uniform vec3 bottomColor;
                uniform float offset;
                uniform float exponent;

                in vec3 vWorldPosition;
                out vec4 fragColor;

                void main() {
                    float h = normalize( vWorldPosition + offset ).y;
                    fragColor = vec4( mix( bottomColor, topColor, max( pow( max( h , 0.0), exponent ), 0.0 ) ), 1.0 );
                }"
            uniforms: {"topColor": Three.color("#0077ff"), "bottomColor": Three.color("#ffffff"), "offset": 33, "exponent": 0.6 }
        }
        Scene {
            id: scene
            bgColor: Qt.hsla(0.6,0,1,1)
            fog: Fog {near: 1; far: 5000; color: Qt.hsla(0.6,0,1,1)}

            Sphere {
                radius: 4000
                widthSegments: 32
                heightSegments: 15

                material: skyMaterial
            }

            ModelRef {
                id: eagle
                model: eagleModel
                type: ModelRef.Node
                castShadow: true
                receiveShadow: true
                scale: "0.035,0.035,0.035"
                position: "30,-5,0"
                rotation: "-1.62716,-0.0526228,-2.53596"
            }

            Plane {
                type: Three.Buffer
                width: 10000
                height: 10000

                rotation.x: -Math.PI / 2
                position.y: -33

                material: MeshPhongMaterial {
                    color: Qt.hsla(0.095,1,0.75,1)
                    specular: "#050505"
                    dithering: true
                }
                receiveShadow: true
            }

            HemisphereLight {
                id: hemilight
                skyColor: Qt.hsla(0.6, 1, 0.6, 1)
                groundColor: Qt.hsla(0.095, 1, 0.75, 1)
                intensity: 0.6
                position: "0,50,0"

                helper.size: 10
            }

            DirectionalLight {
                id: dirlight
                color: Qt.hsla(0.1, 1, 0.95, 1)
                intensity: 1
                position: Qt.vector3d(-1,1.75,1).times(30)
                castShadow: true

                shadow.mapSize: "2048x2048"
                shadow.bias: -0.0001
                shadow.camera.left: -150
                shadow.camera.right: 150
                shadow.camera.top: 150
                shadow.camera.bottom: -150
                shadow.camera.far: 3500

                helper.size: 10
            }

            camera: PerspectiveCamera {
                fov: 30
                aspect: threeD.width / threeD.height
                near: 1
                far: 10000
                position: "0,0,250"

                target: scene.position

                controller: OrbitController {
                    minDistance: 50
                    maxDistance:3500
                    enablePan: false
                }
            }
        }
    }
}
