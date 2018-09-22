import QtQuick 2.7
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.1
import QtQuick.Window 2.2

import three.quick 1.0

Window {
    id: mainWindow
    minimumWidth: 1536
    minimumHeight: 1044
    visible: true

    OptionsMenu {
        title: "Lights"
        anchors.top: parent.top
        anchors.right: parent.right
        width: 200
        color: "transparent"
        z: 2
        threeD: threeD

        BoolChoice {
            name: "Spotlight"
            value: true
            onValueChanged: {
                spotLight.visible = value
                spotLight.shadow.camera.helper.visible = value
                spotShadowViewer.enabled = value
                threeD.update()
            }
        }
        BoolChoice {
            name: "Directional Light"
            value: false
            onValueChanged: {
                dirLight.visible = value
                dirLight.shadow.camera.helper.visible = value
                dirShadowViewer.enabled = value
                threeD.update()
            }
        }
    }
    ThreeD {
        id: threeD
        anchors.fill: parent
        shadowType: Three.Basic

        ShadowMapViewer {
            id: spotShadowViewer
            scale: 0.2
            position: "10,10"
            light: spotLight
            enabled: true
        }
        ShadowMapViewer {
            id: dirShadowViewer
            scale: 0.2
            position: "350,10"
            light: dirLight
            enabled: false
        }

        MeshPhongMaterial {
            id: material
            color: "#ff0000"
            shininess: 150
            specular: "#222222"
        }

        Scene {
            id: scene
            bgColor: "gray"

            AmbientLight {
                color: "#404040"
            }

            SpotLight {
                id: spotLight
                color: "#ffffff"
                intensity: 1.5
				angle: Math.PI / 5
				penumbra: 0.3
				castShadow: true
				shadow.camera.near: 8
				shadow.camera.far: 30
				shadow.mapSize:"1024x1024"

				shadow.camera.helper.visible: true

				position: "10,10,5"
            }

            DirectionalLight {
                id: dirLight
                color: "#ffffff"
                intensity: 1
                castShadow: true
                visible: false
                shadow.camera.near: 1;
				shadow.camera.far: 10
				shadow.camera.right: 15
				shadow.camera.left: -15
				shadow.camera.top: 15
				shadow.camera.bottom: -15
				shadow.mapSize: "1024x1024"

                position: "0,10,0"
				shadow.camera.helper.visible: false
            }

            Torus {
                id: torus
                name: "torus"
                type: Three.BufferGeometry
                radius: 3
                tube: 0.7
                radialSegments: 8
                tubularSegments: 24
                castShadow: true
                receiveShadow: true
                position: "0,4,0"
                rotation: "2,4,6"

                material: material
            }

            Box {
                id: box
                name: "box"
                type: Three.BufferGeometry
                width: 3
                height: 3
                depth: 3
                castShadow: true
                receiveShadow: true

                position: "8,3,8"

                material: material
            }

            Box {
                id: ground
                name: "plane"
                type: Three.BufferGeometry
                width: 10
                height: 0.15
                depth: 10
                scale: "3,3,3"
                castShadow: false
                receiveShadow: true

				material: MeshPhongMaterial {
					color: "#a0adaf"
					shininess: 150
					specular: "#111111"
				}
		    }

            camera: PerspectiveCamera {
                name: "pcamera"
                fov: 45
                aspect: threeD.width / threeD.height
                near: 1
                far: 1000

                position: "0,15,35"

                target: scene.position

                controller: OrbitController {}
            }
        }
    }
}
