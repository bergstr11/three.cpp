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
        id: boxFactory
        Box {
            width: 50
            height: 50
            depth: 50

            material: MeshLambertMaterial {
                id: boxMaterial
                color: Three.randomColor(0xffffff)
            }
        }
    }

    ThreeD {
        id: threeD
        anchors.fill: parent
        focus: true
        shadowType: Three.PCFSoft

        DragController {
            id: dragger
            camera: scene.camera
        }

        Scene {
            id: scene
            bgColor: "#f0f0f0"

            AmbientLight {
                color: "#505050"
            }
            SpotLight {
                color: "#ffffff"
                intensity: 1.5
                position: "0,500,2000"
				angle: Math.PI / 9
				castShadow: true
				shadow.camera.near: 1000
				shadow.camera.far: 4000
				shadow.mapSize:"1024x1024"
            }
            camera: PerspectiveCamera {
                fov: 70
                aspect: threeD.width / threeD.height

                position: "0,0,1000"

                target: scene.position
            }
        }
        Component.onCompleted: {
            var objects = new Array()
            for ( var i = 0; i < 100; i ++ ) {
                var box = boxFactory.createObject(scene)
                box.scale.x = Math.random() * 2 + 1;
                box.scale.y = Math.random() * 2 + 1;
                box.scale.z = Math.random() * 2 + 1;
                box.position.x = Math.random() * 1000 - 500;
                box.position.y = Math.random() * 600 - 300;
                box.position.z = Math.random() * 800 - 400;
                box.rotation.x = Math.random() * 2 * Math.PI;
                box.rotation.y = Math.random() * 2 * Math.PI;
                box.rotation.z = Math.random() * 2 * Math.PI;
                box.castShadow = true
                box.receiveShadow = true

                scene.add( box );
                objects.push( box );
			}
			dragger.objects = objects
        }
    }
}
