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
        id: voxel
        Box {
            width: 50
            height: 50
            depth: 50

            material: MeshLambertMaterial {
                color: "#feb74c"
                map: ImageTexture {
                    image: Image {url: ":/square-outline-textured.png"; format: Image.RGBA8888}
                }
            }
        }
    }

    ThreeD {
        id: threeD
        anchors.fill: parent
        focus: true
        z: 1

        property var objectList: []

        Raycaster {
            id: raycaster
        }

        Scene {
            id: scene
            background: "#f0f0f0"

            AmbientLight {
                id: ambientLight
                color: "#606060"
            }
            DirectionalLight {
                color: "#ffffff"
                position: "1,0.75,0.5"
            }

            Box {
                id: rollOver
                width: 50
                height: 50
                depth: 50

                material: MeshBasicMaterial {
                    color: "#ff0000"
                    opacity: 0.5
                }
            }

            GridLines {
                size: 1000
                divisions: 20
            }

            Plane {
                name: "plane"
                type: Three.BufferGeometry
                width: 1000
                height: 1000

                material: MeshBasicMaterial {visible: false}

                Component.onCompleted: {
                    rotateX(-Math.PI / 2)
                    threeD.objectList.push(this)
                }
            }

            camera: PerspectiveCamera {
                fov: 45
                aspect: threeD.width / threeD.height
                near: 1
                far: 10000

                lookAt: scene.position

                position: "500,800,1300"
            }
        }
    }
    MouseArea {
        anchors.fill: threeD
        onPositionChanged: {
            mouse.accepted = true

            var m = Qt.vector2d((mouse.x / threeD.width) * 2 - 1, -(mouse.y / threeD.height) * 2 + 1)

            raycaster.setFromCamera( m, camera );

            var intersects = raycaster.intersectObjects( objects );
            if ( intersects.length > 0 ) {
                var intersect = intersects[ 0 ];
                rollOver.position = intersect.point + intersect.face.normal
                rollOver.position = (rollOver.position / 50 ).floor() * 50 + 25
            }
            threeD.update()
        }
    }
}
