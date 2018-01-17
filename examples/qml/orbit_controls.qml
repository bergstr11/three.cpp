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

    Component {
        id: cylinderCompo

        Cylinder {
            radiusTop: 0
            radiusBottom: 10
            height: 30
            radialSegments: 4
            heightSegments: 1
        }
    }
    ThreeD {
        id: threeD
        anchors.fill: parent
        focus: true

        MeshPhongMaterial {
            id: material
            color: "#ffffff"
            flatShading: true
        }

        Scene {
            id: scene
            background: "#cccccc"
            fog: FogExp2 {
                color: "#cccccc"
                density: 0.002
            }

            DirectionalLight {
                color: "#ffffff"
                position: "1,1,1"
            }
            DirectionalLight {
                color: "#002288"
                position: "-1,-1,-1"
            }
            AmbientLight {
                color: "#222222"
                intensity: 1
            }
            camera: PerspectiveCamera {
                fov: 60
                aspect: threeD.width / threeD.height
                near: 1
                far: 1000

                position: "0,0,500"

                lookAt: scene.position

                controller: OrbitController {
                    enablePan: true
                }
            }
        }
        Component.onCompleted: {
            for ( var i = 0; i < 500; i ++ ) {
                var pos = Qt.vector3d((Math.random() - 0.5) * 1000,
                                      (Math.random() - 0.5) * 1000,
                                      (Math.random() - 0.5) * 1000)

                var cylinder = cylinderCompo.createObject(
                            scene, {material: material, position: pos, matrixAutoUpdate: false})
                scene.add(cylinder)
            }
        }
    }
}
