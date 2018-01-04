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
        anchors.fill: parent
        color: "lightgray"
    }

    OptionsMenu {
        anchors.top: parent.top
        anchors.right: parent.right
        color: "transparent"
        width: 350
        height: 200
        z: 2

        MenuChoice {
            name: "Cube"
            value: true
            onSelected: {
                skyBox.material = cubeMaterial
				skyBox.visible = true
                sphere.material.envMap = textureCube

				sphere.material.needsUpdate = true
                threeD.update()
            }
        }
        MenuChoice {
            name: "Equirectangular"
            onSelected: {
                skyBox.material = equirectMaterial
                skyBox.visible = true
                sphere.material.envMap = textureEquirec

				sphere.material.needsUpdate = true
                threeD.update()
            }
        }
        MenuChoice {
            name: "Spherical"
            onSelected: {
                skyBox.visible = false
                sphere.material.envMap = textureSphere

				sphere.material.needsUpdate = true
                threeD.update()
            }
        }
        BoolChoice {
            name: "Refraction"
            value: false
            onValueChanged: {
                if ( value ) {
                    textureEquirec.mapping = Texture.EquirectangularRefraction
                    textureCube.mapping = Texture.CubeRefraction
                }
                else {
                    textureEquirec.mapping = Texture.EquirectangularReflection
                    textureCube.mapping = Texture.CubeReflection
                }
				sphere.material.needsUpdate = true
                threeD.update()
            }
        }
    }
    ThreeD {
        id: threeD
        anchors.fill: parent
        faceCulling: Three.NoFaceCulling
        autoClear: false

        ImageCubeTexture {
            id: textureCube
            format: Texture.RGBA
            mapping: Texture.CubeReflection
            flipY: false
            type: Texture.UnsignedByte
            images: [
                Image {url: ":/cube_bridge2_posx.jpg"; format: Image.RGBA8888},
                Image {url: ":/cube_bridge2_negx.jpg"; format: Image.RGBA8888},
                Image {url: ":/cube_bridge2_posy.jpg"; format: Image.RGBA8888},
                Image {url: ":/cube_bridge2_negy.jpg"; format: Image.RGBA8888},
                Image {url: ":/cube_bridge2_posz.jpg"; format: Image.RGBA8888},
                Image {url: ":/cube_bridge2_negz.jpg"; format: Image.RGBA8888}
            ]
        }
        ImageTexture {
            id: textureSphere
            format: Texture.RGBA
            mapping: Texture.SphericalReflection
            image: Image {url: ":/metal.jpg"; format: Image.RGBA8888}
        }
        ImageTexture {
            id: textureEquirec
            format: Texture.RGBA
            mapping: Texture.EquirectangularReflection
            magFilter: Texture.Linear
            minFilter: Texture.LinearMipMapLinear
            image: Image {url: ":/2294472375_24a3b8ef46_o.jpg"; format: Image.RGBA8888}
        }

        ShaderMaterial {
            id: equirectMaterial
            shaderID: "equirect"
            side: Three.BackSide
            depthTest: false
            depthWrite: false
            uniforms: {"tEquirect": textureEquirec}
        }

        ShaderMaterial {
            id: cubeMaterial
            shaderID: "cube"
            side: Three.BackSide
            depthTest: false
            depthWrite: false
            uniforms: {"tCube": textureCube}
        }

        Scene {
            id: sceneCube
            name: "scene_cube"

            Box {
                id: skyBox
                width: 100
                height: 100
                depth: 100

                material: cubeMaterial
            }
            camera: PerspectiveCamera {
                fov: 70
                aspect: threeD.width / threeD.height
                near: 1
                far: 100000

                rotation: sceneCamera.rotation
            }
        }

        Scene {
            id: scene
            name: "scene_sphere"

            AmbientLight {
                id: ambientLight
                color: "#ffffff"
            }

            Sphere {
                id: sphere
                radius: 400
                widthSegments: 48
                heightSegments: 24

                material: MeshLambertMaterial {envMap: textureCube}
            }

            camera: PerspectiveCamera {
                id: sceneCamera
                fov: 70
                aspect: threeD.width / threeD.height
                near: 1
                far: 100000

                position: "0,0,1000"

                lookAt: scene.position

                controller: OrbitController {
                    minDistance: 500
                    maxDistance: 2500
                    enablePan: false
                }
            }
        }
    }
}
