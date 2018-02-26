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
            name: "Orthographic"
            otherName: "Perspective"
            value: false
            onValueChanged: {

                if(value === true) {
                    activeCamera = cameraOrtho
                }
                else {
                    activeCamera = cameraPerspective
                }
                threeD.update()
            }
        }
    }

    ThreeD {
        id: threeD
        anchors.fill: parent
        focus: true
        antialias: true
        autoClear: false

        property float frustumSize: 600
        property int frameCount: 0
        property Camera activeCamera

        Scene {
            id: scene

            Sphere {
                id: sphere
                type: Three.BufferGeometry
                radius: 100
                widthSegments: 16
                heightSegments: 8

                material: MeshBasicMaterial {
                    color: "#ffffff"
                    wireframe: true
                }

                Sphere {
                    id: sphere2
                    type: Three.BufferGeometry
                    radius: 50
                    widthSegments: 16
                    heightSegments: 8
                    position.y: 150

                    material: MeshBasicMaterial {
                        color: "#00ff00"
                        wireframe: true
                    }
                }
            }

            Points {
                id: particles
                material: PointsMaterial {color: "#888888"}

                Component.onCompleted: {
                    for ( var i = 0; i < 10000; i ++ ) {

                        var vertex = Qt.vector3();
                        vertex.x = THREE.Math.randFloatSpread( 2000 );
                        vertex.y = THREE.Math.randFloatSpread( 2000 );
                        vertex.z = THREE.Math.randFloatSpread( 2000 );

                        geometry.vertices.push( vertex );
				    }
				}
            }

            PerspectiveCamera {
                id: defaultCamera
                fov: 50
                aspect: 0.5 * threeD.width / threeD.height
                near: 1
                far: 10000
                position: "0,0,2500"
            }

            Group {
                id: cameraRig
                Sphere {
                    id: sphere3
                    type: Three.BufferGeometry
                    radius: 5
                    widthSegments: 16
                    heightSegments: 8
                    position.z: 150

                    material: MeshBasicMaterial {
                        color: "#0000ff"
                        wireframe: true
                    }
                }

                PerspectiveCamera {
                    id: cameraPerspective
                    fov: 50
                    aspect: 0.5 * threeD.width / threeD.height
                    near: 150
                    far: 1000
                    rotation.y: Math.PI

                    helper.visible: true
                }
                OrthographicCamera {
                    id: cameraOrtho
                    left: 0.5 * frustumSize * aspect / - 2
                    right: 0.5 * frustumSize * aspect / 2
                    top: frustumSize / 2
                    bottom: frustumSize / - 2
                    near: 150
                    far: 1000
                    rotation.y: Math.PI

                    helper.visible: true
                }
            }
        }

        onSizeChanged: {
            camera.updateProjectionMatrix()
            cameraPerspective.updateProjectionMatrix()
            cameraOrtho.updateProjectionMatrix()
        }

        onBeforeRender: {

            if((frameCount % 2) == 0) {
                var r = Date.now() * 0.0005;

                sphere.position.x = 700 * Math.cos( r );
                sphere.position.z = 700 * Math.sin( r );
                sphere.position.y = 700 * Math.sin( r );

                sphere2.position.x = 70 * Math.cos( 2 * r );
                sphere2.position.z = 70 * Math.sin( r );

                if (scene.camera === cameraPerspective) {

                    cameraPerspective.fov = 35 + 30 * Math.sin( 0.5 * r )
                    cameraPerspective.far = mesh.position.length()
                    cameraPerspective.updateProjectionMatrix()

                    cameraPerspective.helper.update()
                    cameraPerspective.helper.visible = true

                    cameraOrtho.helper.visible = false;
                }
                else {

                    cameraOrtho.far = mesh.position.length()
                    cameraOrtho.updateProjectionMatrix()

                    cameraOrtho.helper.update()
                    cameraOrtho.helper.visible = true

                    cameraPerspective.helper.visible = false
                }

                renderer.viewport = Qt.rectangle(0,0,threeD.width/2,threeD.height);
                scene.camera = activeCamera
            }
            else {
                renderer.viewport = Qt.rectangle(threeD.width/2,0,threeD.width/2,threeD.height);
                scene.camera = defaultCamera
            }

            cameraRig.lookAt( sphere.position );

            renderer.clear();
        }

        onRendered: update()
    }
}
