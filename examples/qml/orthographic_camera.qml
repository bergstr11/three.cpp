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
    color: "black"

    Component {
        id: boxFactory
        Box {
            width: 50
            height: 50
            depth: 50

            material: MeshLambertMaterial {
                id: boxMaterial
                color: "#ffffff"
            }
        }
    }

    ThreeD {
        id: threeD
        anchors.fill: parent
        focus: true
        antialias: true
        autoClear: false

        property real frustumSize: 1000
        property real aspect: width / height

        Scene {
            id: scene
            background: "#f0f0f0"

            GridLines {
                id: grid
                size: 1000
                divisions: 20
            }

            AmbientLight {
                color: Three.randomColor(0x101010)
            }

            DirectionalLight {
                color: Three.randomColor(0xffffff)

				Component.onCompleted: {
                    position = Qt.vector3d(Math.random() - 0.5, Math.random() - 0.5, Math.random() - 0.5).normalized()
				}
			}
            DirectionalLight {
                color: Three.randomColor(0xffffff)

				Component.onCompleted: {
                    position = Qt.vector3d(Math.random() - 0.5, Math.random() - 0.5, Math.random() - 0.5).normalized()
				}
			}

            camera: OrthographicCamera {
                left: threeD.frustumSize * threeD.aspect / -2
                right: threeD.frustumSize * threeD.aspect / 2
                top: threeD.frustumSize / 2
                bottom: threeD.frustumSize / - 2
                near: 1
                far: 2000
                position.y: 350
                target: scene.position
            }

			Component.onCompleted: {
                for ( var i = 0; i < 100; i ++ ) {
					var cube = boxFactory.createObject(scene)
					cube.scale.y = Math.floor( Math.random() * 2 + 1 );
					cube.position.x = Math.floor( ( Math.random() * 1000 - 500 ) / 50 ) * 50 + 25;
					cube.position.y =  ( cube.scale.y *  50 ) / 2;
					cube.position.z = Math.floor( ( Math.random() * 1000 - 500 ) / 50 ) * 50 + 25;
					scene.add( cube );
				}
			}
        }

        onGeometryChanged: {
            scene.camera.updateProjectionMatrix()
        }

        animate: function() {
            var timer = Date.now() * 0.0001;

            scene.camera.position.x = Math.cos( timer ) * 800;
            scene.camera.position.z = Math.sin( timer ) * 800;
            scene.camera.lookAt(scene.position);
	    }
    }
}
