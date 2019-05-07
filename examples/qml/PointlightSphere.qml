import three.quick 1.0

PointLight {
    id: pointLight
    intensity: 1.5
    distance: 30
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
            alphaMap: ColorTexture {
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
            alphaMap: sphere2.material.alphaMap
            alphaTest: sphere2.material.alphaTest
        }
    }
}
