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

        FloatChoice {
            label: "exposure:"
            from: 0
            to: 1
            value: 0.68
            onValueChanged: {
                threeD.toneMappingExposure = Math.pow( value, 5.0 );
                threeD.update()
            }
        }
        BoolChoice {
            label: "shadows:"
            value: true
            onValueChanged: {
                threeD.shadowMap.type = value ? Three.BasicShadow : Three.NoShadow
                ball.material.needsUpdate = true
                floor.material.needsUpdate = true
                cubeMat.needsUpdate = true
                threeD.update()
            }
        }
        ListChoice {
            id: bulbLuminousPowers
            label: "bulbPower:"
            names: ["110000 lm (1000W)",
            		"3500 lm (300W)",
            		"1700 lm (100W)",
            		"800 lm (60W)",
            		"400 lm (40W)",
            		"180 lm (25W)",
            		"20 lm (4W)",
            		"Off"]
            values: [110000, 3500, 1700, 800, 400, 180, 20, 0]
            selectedIndex: 4

            onSelectedIndexChanged: {
                bulbLight.power = values[ selectedIndex ];
				threeD.update()
            }
            function selectedValue() {return values[selectedIndex]}
        }
        ListChoice {
            id: hemiLuminousIrradiances
            label: "hemiIrradiance:"
            names: ["0.0001 lx (Moonless Night)",
                    "0.002 lx (Night Airglow)",
                    "0.5 lx (Full Moon)",
                    "3.4 lx (City Twilight)",
                    "50 lx (Living Room)",
                    "100 lx (Very Overcast)",
                    "350 lx (Office Room)",
                    "400 lx (Sunrise/Sunset)",
                    "1000 lx (Overcast)",
                    "18000 lx (Daylight)",
                    "50000 lx (Direct Sun)"]
            values: [0.0001, 0.002, 0.5, 3.4, 50, 100, 350, 400, 1000, 18000, 50000]
            selectedIndex: 0

            onSelectedIndexChanged: {
                hemiLight.intensity = values[ selectedIndex ];
				threeD.update()
            }
            function selectedValue() {return values[selectedIndex]}
        }
        BoolChoice {
            name: "Animate"
            value: false
            onValueChanged: {
                threeD.runAnimation(value)
            }
        }
    }

    ThreeD {
        id: threeD
        anchors.fill: parent
        focus: true
        physicallyCorrectLights: true
        gammaInput: true
        gammaOutput: true
        toneMapping: Three.ReinhardToneMapping
        toneMappingExposure: Math.pow( 0.68, 5.0 );
        shadowMap.type: Three.PCFShadow
        autoAnimate: false

        MeshStandardMaterial {
            id: cubeMat
            roughness: 0.7
            color: "#ffffff"
            metalness: 0.2
            bumpScale: 0.002

            map: ImageTexture {
                wrapS: Three.RepeatWrapping
                wrapT: Three.RepeatWrapping
                anisotropy: 4
                repeat: "1,1"
                image: ":/brick_diffuse.jpg"
            }
            bumpMap: ImageTexture {
                wrapS: Three.RepeatWrapping
                wrapT: Three.RepeatWrapping
                anisotropy: 4
                repeat: "1,1"
                image: ":/brick_bump.jpg"
            }
        }

        ShadowMapViewer {
            id: spotShadowViewer
            scale: 0.4
            position: "700,10"
            light: bulbLight
            enabled: bulbLight.visible
        }

        Scene {
            id: scene

            PointLight {
                id: bulbLight
                color: "#ffee88"
                distance: 100
                decay: 2
                position: "0,2,0"
                power: bulbLuminousPowers.selectedValue()
                castShadow: true

                Sphere {
                    id: bulb
                    type: Three.BufferGeometry
                    radius: 0.02
                    widthSegments: 18
                    heightSegments: 8

                    material: MeshStandardMaterial {
                        emissive: "#ffffee"
                        emissiveIntensity: bulbLight.intensity / Math.pow( 0.02, 2.0 );
                        color: "#000000"
                    }
                }
            }

            HemisphereLight {
                id: hemiLight
                skyColor: "#ddeeff"
                groundColor: "#0f0e0d"
                intensity: hemiLuminousIrradiances.selectedValue()
            }

            Plane {
                id: floor
                type: Three.BufferGeometry
                width: 20
                height: 20
                receiveShadow: true
                rotation.x: - Math.PI / 2.0

                material: MeshStandardMaterial {
                    id: floorMat
                    roughness: 0.8
                    color: "#ffffff"
                    metalness: 0.2
                    bumpScale: 0.0005

                    map: ImageTexture {
                        wrapS: Three.RepeatWrapping
                        wrapT: Three.RepeatWrapping
                        anisotropy: 4
                        repeat: "10,24"
                        image: ":/hardwood2_diffuse.jpg"
                    }
                    bumpMap: ImageTexture {
                        wrapS: Three.RepeatWrapping
                        wrapT: Three.RepeatWrapping
                        anisotropy: 4
                        repeat: "10,24"
                        image: ":/hardwood2_bump.jpg"
                    }
                    roughnessMap: ImageTexture {
                        wrapS: Three.RepeatWrapping
                        wrapT: Three.RepeatWrapping
                        anisotropy: 4
                        repeat: "10,24"
                        image: ":/hardwood2_roughness.jpg"
                    }
                }
            }
            Sphere {
                id: ball
                type: Three.BufferGeometry
                radius: 0.25
                widthSegments: 32
                heightSegments: 32
                position: "1,0.25,1"
                rotation.y: Math.PI
                castShadow: true

                material: MeshStandardMaterial {
                    id: ballMat
                    roughness: 0.5
                    color: "#ffffff"
                    metalness: 1.0

                    map: ImageTexture {
                        anisotropy: 4
                        image: ":/earth_atmos_2048.jpg"
                    }
                    metalnessMap: ImageTexture {
                        anisotropy: 4
                        image: ":/earth_specular_2048.jpg"
                    }
                }
            }

            Box {
                id: box1
                type: Three.BufferGeometry
                width: 0.5
                height: 0.5
                depth: 0.5
                castShadow: true
                position: "-0.5,0.25,-1"

                material: cubeMat
            }
            Box {
                id: box2
                type: Three.BufferGeometry
                width: 0.5
                height: 0.5
                depth: 0.5
                castShadow: true
                position: "0,0.25,-5"

                material: cubeMat
            }
            Box {
                id: box3
                type: Three.BufferGeometry
                width: 0.5
                height: 0.5
                depth: 0.5
                castShadow: true
                position: "7,0.25,0"

                material: cubeMat
            }

            camera: PerspectiveCamera {
                fov: 50
                aspect: threeD.width / threeD.height
                near: 0.1
                far: 100
                position: "-4,4,2"

                target: scene.position

                controller: OrbitController {}
            }
        }
        animate: function() {
            var time = Date.now() * 0.0005;
			bulbLight.position.y = Math.cos( time ) * 0.75 + 1.25;
        }
    }
}
