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

    /*Chooser {
        anchors.top: parent.top
        anchors.right: parent.right
        width: 350
        height: 200

        Choice {
            text: "Cube"
            group: "configs"
            onSelected: {
                skyBox.material = cubeMaterial
				skyBox.visible = true
				sphere.material.envMap = textureCube
				sphere.material.needsUpdate = true
            }
        }
        Choice {
            text: "Equirectangular"
            onSelected: {
                skyBox.material = equirectMaterial
                skyBox.visible = true
                sphere.material.envMap = textureEquirec
                sphere.material.needsUpdate = true
            }
        }
        Choice {
            text: "Spherical"
            onSelected: {
                cubeMesh.visible = false
                sphereMaterial.envMap = textureSphere
                sphereMaterial.needsUpdate = true
            }
        }
        BoolChoice {
            text: "Refraction"
            value: false
            onSelected: {
                if ( value ) {
                    textureEquirec.mapping = Texture.EquirectangularRefraction
                    textureCube.mapping = Texture.CubeRefraction
                }
                else {
                    textureEquirec.mapping = Texture.EquirectangularReflection
                    textureCube.mapping = Texture.CubeReflection
                }
				sphereMaterial.needsUpdate = true
            }
        }
    }*/
    ThreeD {
        anchors.fill: parent

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
                Image {url: ":/cube_bridge2_posx.jpg"; format: Image.RGBA8888},
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
            shaderID: "cube"
            side: Three.BackSide
            depthTest: false
            depthWrite: false
            uniforms: {"tEquirect": textureEquirec}
        }

        Scene {
            id: sceneCube

            Box {
                id: skyBox
                width: 100
                height: 100
                depth: 100

                material: ShaderMaterial {
                    shaderID: "cube"
                    side: Three.BackSide
                    depthTest: false
                    depthWrite: false
                    uniforms: {"tCube": textureCube}
                }
            }
            camera: PerspectiveCamera {
                fov: 70
                aspect: scene.width / scene.height
                near: 1
                far: 100000
            }
        }

        Scene {
            id: scene

            AmbientLight {
                id: ambientLight
                color: "#ffffff"
            }

            Sphere {
                id: sphere
                radius: 400
                widthSegments: 48
                heightSegments: 24

                material: MeshLambertMaterial {
                    envMap: textureCube
                }
            }

            camera: PerspectiveCamera {
                fov: 70
                aspect: scene.width / scene.height
                near: 1
                far: 100000

                position: "0,0,1000"

                lookAt: scene

                controller: OrbitController {
                    minDistance: 500
                    maxDistance: 2500
                    enablePan: false
                }
            }
        }
    }
}
