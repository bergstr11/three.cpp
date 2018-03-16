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
            type: Three.BufferGeometry
            width: 20
            height: 20
            depth: 20

            property alias color: boxMaterial.color

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
        fps: 40

        property real aspect: width / height

        property int radius: 500
        property real theta: 0
        property int frustumSize: 1000

        ObjectPicker
        {
            id: objectPicker
            camera: scene.camera
            objects: [scene]
            recurse: true

            onObjectsClicked: {
                var intersected = intersect(0).object
                intersected.material.emissive = "#ff0000"
            }
            onObjectsDoubleClicked: {
                var intersected = intersect(0).object
                intersected.material.emissive = "#0000dd"
                intersected.scale = "1.5,1.5,1.5"
            }
        }

        Scene {
            id: scene
            background: "#f0f0f0"

            DirectionalLight {
                color: "#ffffff"
                position: Qt.vector3d(1,1,1).normalized()
            }

            camera: OrthographicCamera
            {
                left: threeD.frustumSize * threeD.aspect / - 2
                right: threeD.frustumSize * threeD.aspect / 2
                top: threeD.frustumSize / 2
                bottom: threeD.frustumSize / - 2
                near: 1
                far: 1000
            }
        }

        Component.onCompleted: {

            for ( var i = 0; i < 2000; i ++ ) {
                var object = boxFactory.createObject(threeD, {color: Three.randomColor(0xffffff)})
                object.position.x = Math.random() * 800 - 400;
                object.position.y = Math.random() * 800 - 400;
                object.position.z = Math.random() * 800 - 400;
                object.rotation.x = Math.random() * 2 * Math.PI;
                object.rotation.y = Math.random() * 2 * Math.PI;
                object.rotation.z = Math.random() * 2 * Math.PI;
                object.scale.x = Math.random() + 0.5;
                object.scale.y = Math.random() + 0.5;
                object.scale.z = Math.random() + 0.5;
                scene.add( object );
			}
        }

        animate: function() {
            theta += 0.1;

            scene.camera.position.x = radius * Math.sin(Three.DEG2RAD * theta);
            scene.camera.position.y = radius * Math.sin(Three.DEG2RAD * theta);
            scene.camera.position.z = radius * Math.cos(Three.DEG2RAD * theta);
            scene.camera.lookAt( scene.position );
            scene.camera.updateMatrixWorld();
        }
    }
}
