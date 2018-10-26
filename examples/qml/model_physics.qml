import QtQuick 2.7
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.1
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.0

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
    Row {
        id: lrow
        anchors.top: parent.top
        anchors.topMargin: 10
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: 20
        z: 2

        Label {id: dlabel; text: "Distance: %1".arg(orbitController.distance)}
        Label {id: rlabel; text: "Rotation: %1:%2:%3".arg(physicstest.rotation.x).arg(physicstest.rotation.y).arg(physicstest.rotation.z)}

    }
    Row {
        id: dataRow
        anchors.top: lrow.bottom; anchors.topMargin: 10; anchors.horizontalCenter: lrow.horizontalCenter
        spacing: 10
        z: 2

        ComboBox {
            id: pickedParents

            onModelChanged: parent.checkEnablement()
        }

        property var picked

        function checkEnablement()
        {
            hideButton.enabled = saveButton.enabled = !!picked && pickedParents.currentIndex >= 0
        }

        onPickedChanged: checkEnablement()

        Button {
            id: hideButton
            text: "Hide"
            enabled: false

            onClicked: {
                if(!!dataRow.picked) {
                    var parent = dataRow.picked.parentObject(pickedParents.model[pickedParents.currentIndex])
                    parent.visible = false
                }
                threeD.update()
            }
        }
        Button {
            id: saveButton
            text: "Save"
            enabled: false

            onClicked: {
                if(!!dataRow.picked) {
                    var parent = pickedParents.currentIndex > 0 ?
                        dataRow.picked.parentObject(pickedParents.model[pickedParents.currentIndex]) : dataRow.picked

                    if(physicstest.picked1 === null) {
                        physicstest.picked1 = parent
                        textO1.text = parent.name
                    }
                    else {
                        physicstest.picked2 = parent
                        textO2.text = parent.name
                    }
                }
            }
        }
    }
    Column {
        id: infoRow
        anchors.right: parent.right
        anchors.bottom: actions.top
        anchors.margins: 10
        spacing: 10

        Text {id: textO1}
        Text {id: textO2}
        Text {id: textP1}
        Text {id: textP2}
    }
    Column {
        id: actions
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: 10
        spacing: 10
        z: 2
    
        Button {
            id: createButton
            text: "Create"
            enabled: false
            onClicked: physicstest.createTheHinge()
        }
        Button {
            id: resetButton
            text: "Reset"
            enabled: false
            onClicked: physicstest.resetPicked()
        }
    }

    MouseArea {
        anchors.fill: lrow
        z: 3
        onClicked: {
            console.log(dlabel.text, rlabel.text)
        }
    }

    ColorDialog {
        id: colorDialog
        title: "Please choose a color"
        visible: false

        property Light light
        property string colorName: "color"

        onVisibleChanged: {
            if(visible) {
                color = light[colorName]
            }
        }
        onAccepted: {
            if(light) {
                light[colorName] = colorDialog.color
                threeD.update()
            }
        }
    }

    OptionsMenu {
        id: lightControls
        title: "light"
        anchors.top: parent.top
        anchors.left: parent.left
        width: 350
        color: "transparent"
        z: 2
        threeD: threeD

        FloatValue {
            label: "hemisphere"
            name: "intensity"
            target: hemisphereLight
            from: 0
            to: 2
        }
        FloatValue {
            label: "directional"
            name: "intensity"
            target: directionalLight
            from: 0
            to: 2
        }
        MenuChoice {
            name: "hemisphere sky color"
            isAction: true
            onSelected: {
                colorDialog.light = hemisphereLight
                colorDialog.colorName = "skyColor"
                colorDialog.visible = true
            }
        }
        MenuChoice {
            name: "hemisphere ground color"
            isAction: true
            onSelected: {
                colorDialog.light = hemisphereLight
                colorDialog.colorName = "groundColor"
                colorDialog.visible = true
            }
        }
        MenuChoice {
            name: "directional color"
            isAction: true
            onSelected: {
                colorDialog.light = directionalLight
                colorDialog.colorName = "color"
                colorDialog.visible = true
            }
        }
    }
    OptionsMenu {
        id: objectControls
        title: "adjust model"
        anchors.top: parent.top
        anchors.right: parent.right
        width: 350
        color: "transparent"
        z: 2
        threeD: threeD

        FloatManip {
            name: "rotation.x"
            target: physicstest
            from: -Math.PI
            to: Math.PI
            onValueChanged: {
                target.rotateX(value - base)
                base = value
            }
        }
        FloatManip {
            name: "rotation.y"
            target: physicstest
            from: -Math.PI
            to: Math.PI
            onValueChanged: {
                target.rotateY(value - base)
                base = value
            }
        }
        FloatManip {
            name: "rotation.z"
            target: physicstest
            from: -Math.PI
            to: Math.PI
            onValueChanged: {
                target.rotateZ(value - base)
                base = value
            }
        }
        BoolChoice {
            name: "Convex Hull"
            value: convexHull.visible
            onValueChanged: {
                convexHull.visible = value
                threeD.update()
            }
        }
        BoolChoice {
            name: "Select | Mark"
            id: selectMarkChoice
            value: false
        }
        BoolChoice {
            name: "Pick"
            value: false
            onValueChanged: {
                objectPicker.enabled = value
                scene.camera.controller.enabled = !value
            }
        }
    }

    FileDialog {
        id: fileDialog
        title: "Please choose a model file"
        folder: shortcuts.home
        selectMultiple: false
        selectExisting: true
        onAccepted: {
            threeDModel.file = fileDialog.fileUrl
        }
    }
    Button {
        text: "Choose.."
        z: 2
        onClicked: fileDialog.visible = true
    }

    Label {
        id: holdon
        anchors.centerIn: parent
        text: "Stand by, we're loading.."
        font.pointSize: 34
        visible: false

        RotationAnimation on rotation {
            loops: Animation.Infinite
            from: 0
            to: 360
            duration: 4000
            running: holdon.visible
        }
    }

    ThreeD {
        id: threeD
        anchors.fill: parent
        autoClear: false
        samples: 24

        Model {
            id: threeDModel

            onFileChanged: holdon.visible = true
            onModelLoaded: holdon.visible = false
        }

        ObjectPicker
        {
            id: objectPicker
            camera: scene.camera
            enabled: false
            objects: [scene]

            onObjectsClicked: {
                var is = intersect(0)
                pickedParents.model = objectPicker.pickedParents(0)
                dataRow.picked = is.object

                if(selectMarkChoice.value) {
                    physicstest.setMarker(this, orbitController.polar(), orbitController.azimuth())

                    if(!physicstest.upperSet) {
                        physicstest.upper = is.point
                        physicstest.upperSet = true
                        textP1.text = ""+is.point.x.toFixed(2)+":"+is.point.y.toFixed(2)+":"+is.point.z.toFixed(2)
                    }
                    else {
                        physicstest.lower = is.point
                        physicstest.lowerSet = true
                        textP2.text = ""+is.point.x.toFixed(2)+":"+is.point.y.toFixed(2)+":"+is.point.z.toFixed(2)
                    }
                }
            }
        }

        Scene {
            id: scene

            HemisphereLight {
                id: hemisphereLight
                name: "hemisphereLight"
                skyColor: Qt.hsla(0.6, 1, 0.6, 1)
                groundColor: Qt.hsla(0.095, 1, 0.75, 1)
                intensity: 0.6
                position.y: 250
            }

            ConvexHull {
                id: convexHull
                visible: false

                material: MeshBasicMaterial {color: "green"; wireframe: true}

                PhysicsTestModel {
                    id: physicstest
                    model: threeDModel
                    name: "car"
                    type: ModelRef.Node
                    replace: true

                    property var picked1: null
                    property var picked2: null
                    property vector3d upper: "0,0,0"
                    property vector3d lower: "0,0,0"
                    property bool upperSet: false
                    property bool lowerSet: false

                    function checkEnabled() {
                        createButton.enabled = resetButton.enabled =
                            picked1 !== null && picked2 !== null && upperSet && lowerSet
                    }
                    function resetPicked() {
                        picked1 = null; picked2 = null; upperSet = false; lowerSet = false
                        textO1.text = ""; textO2.text = ""; textP1.text = ""; textP2.text = ""
                        createButton.enabled = resetButton.enabled = false
                    }
                    function createTheHinge() {
                        createHinge(picked1, picked2, upper, lower)
                    }

                    onPicked1Changed: checkEnabled()
                    onPicked2Changed: checkEnabled()
                    onUpperSetChanged: checkEnabled()
                    onLowerSetChanged: checkEnabled()

                    onObjectChanged: {
                        pickedParents.model = []
                        resetPicked()

                        orbitController.reset()
                        objectControls.reset()
                        lightControls.reset()
                    }
                }
            }

            camera: PerspectiveCamera {
                id: sceneCamera
                name: "sceneCamera"
                aspect: threeD.width / threeD.height
                near: 1
                far: 100000

                position: "0,0,1000"

                target: scene.position

                controller: OrbitController {
                    id: orbitController
                    maxPolarAngle: Math.PI;
                    enablePan: true
                    rotateCursor: Qt.ClosedHandCursor
                    panCursor: Qt.SizeAllCursor
                }

                DirectionalLight {
                    id: directionalLight
                    color: Qt.hsla(0.1, 1, 0.95, 1)
                    position: Qt.vector3d(-1.3,1.75,1).times(130)
                    intensity: 0.5
                }
            }
        }
    }
}
