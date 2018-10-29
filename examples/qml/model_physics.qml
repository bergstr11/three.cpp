import QtQuick 2.7
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.1
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.0

import three.quick 1.0

Window {
    id: mainWindow
    title: "Door Hinge Editor"
    minimumWidth: 1580
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
        Label {id: rlabel; text: "Rotation: %1:%2:%3".arg(hingeeditor.rotation.x).arg(hingeeditor.rotation.y).arg(hingeeditor.rotation.z)}

    }
    Row {
        id: dataRow
        anchors.top: lrow.bottom; anchors.topMargin: 10; anchors.horizontalCenter: lrow.horizontalCenter
        spacing: 10
        z: 2

        ComboBox {
            id: pickedParents
        }

        property var picked
        property bool buttonsEnabled: !!picked && pickedParents.currentIndex >= 0

        Button {
            id: hideButton
            text: "Hide"
            enabled: parent.buttonsEnabled

            onClicked: {
                if(!!dataRow.picked) {
                    var parent = dataRow.picked.parentObject(pickedParents.model[pickedParents.currentIndex])
                    parent.visible = false
                    hingeeditor.hidden.push(parent)
                }
                threeD.update()
            }
        }
        Button {
            id: setButton
            text: "Set"
            enabled: parent.buttonsEnabled

            onClicked: {
                if(!!dataRow.picked) {
                    var parent = pickedParents.currentIndex > 0 ?
                        dataRow.picked.parentObject(pickedParents.model[pickedParents.currentIndex]) : dataRow.picked

                    if(hingeeditor.picked1 === null) {
                        hingeeditor.picked1 = parent
                        textO1.text = parent.name
                    }
                    else {
                        hingeeditor.picked2 = parent
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
            id: runButton
            enabled: hingeeditor.hingeNames.length > 0
            text: "Animate"

            property bool running: false

            onClicked: {
                running = !running
                if(running) {
                    text = "Stop"
                    hingeeditor.startTimer()
                }
                else {
                    text = "Animate"
                    hingeeditor.stopTimer()
                }

                threeD.runAnimation(running)
            }
        }
        Button {
            id: loadButton
            text: "Load"
            enabled: hingeeditor.object !== null
            onClicked: {
                fileDialog.title = "Load hinge definition from file"
                fileDialog.selectExisting = true
                fileDialog.acceptedFunc = function() {
                    hingeeditor.loadHingeFile(fileDialog.fileUrl)
                }
                fileDialog.visible = true
            }
        }
        Button {
            id: saveButton
            text: "Save"
            enabled: hingeeditor.hingeNames.length > 0
            onClicked: {
                fileDialog.title = "Save hinge definition to file"
                fileDialog.selectExisting = false
                fileDialog.acceptedFunc = function() {hingeeditor.saveHingeFile(fileDialog.fileUrl)}
                fileDialog.visible = true
            }
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
            target: hingeeditor
            from: -Math.PI
            to: Math.PI
            onValueChanged: {
                target.rotateX(value - base)
                base = value
            }
        }
        FloatManip {
            name: "rotation.y"
            target: hingeeditor
            from: -Math.PI
            to: Math.PI
            onValueChanged: {
                target.rotateY(value - base)
                base = value
            }
        }
        FloatManip {
            name: "rotation.z"
            target: hingeeditor
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
    Column {
        anchors.top: objectControls.bottom
        anchors.right: parent.right
        anchors.margins: 10
        spacing: 10
        z: 2

        ComboBox {
            id: hingeSelector
            model: hingeeditor.hingeNames
        }

        Button {
            id: deleteButton
            text: "Delete"
            enabled: hingeSelector.currentIndex >= 0
            onClicked: {
                hingeeditor.deleteHinge(hingeSelector.model[hingeSelector.currentIndex])
                threeD.update()
            }
        }
        Button {
            id: createButton
            enabled: hingeeditor.dataComplete
            text: "Create"

            onClicked: hingeeditor.create()
        }
        Button {
            id: resetButton
            text: "Reset"
            enabled: hingeeditor.dataStarted
            onClicked: {
                hingeeditor.resetEditor()
                threeD.update()
            }
        }
    }


    FileDialog {
        id: fileDialog
        folder: shortcuts.home
        selectMultiple: false

        property var acceptedFunc

        onAccepted: acceptedFunc()
    }
    Button {
        text: "Choose.."
        z: 2
        onClicked: {
            fileDialog.title = "Please choose a model file"
            fileDialog.selectExisting = true
            fileDialog.acceptedFunc = function() {threeDModel.file = fileDialog.fileUrl}
            fileDialog.visible = true
        }
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
        samples: 24
        autoAnimate: false

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
                    hingeeditor.setMarker(this, orbitController.polar(), orbitController.azimuth())

                    if(!hingeeditor.upperSet) {
                        hingeeditor.upper = is.point
                        hingeeditor.upperSet = true
                        textP1.text = ""+is.point.x.toFixed(2)+":"+is.point.y.toFixed(2)+":"+is.point.z.toFixed(2)
                    }
                    else {
                        hingeeditor.lower = is.point
                        hingeeditor.lowerSet = true
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

                HingeEditorModelRef {
                    id: hingeeditor
                    model: threeDModel
                    name: "car"
                    type: ModelRef.Node
                    replace: true

                    property var hidden: []
                    property var picked1: null
                    property var picked2: null
                    property vector3d upper: "0,0,0"
                    property vector3d lower: "0,0,0"
                    property bool upperSet: false
                    property bool lowerSet: false

                    property bool dataStarted: picked1 !== null || picked2 !== null || upperSet || lowerSet
                    property bool dataComplete: picked1 !== null && picked2 !== null && upperSet && lowerSet

                    function unhide() {
                        for(var i=0; i<hidden.length; i++) hidden[i].visible = true
                        hidden = new Array()
                        removeMarkers()
                        threeD.update()
                    }

                    function resetEditor() {
                        picked1 = null; picked2 = null; upperSet = false; lowerSet = false
                        textO1.text = ""; textO2.text = ""; textP1.text = ""; textP2.text = ""
                        pickedParents.model = []
                        unhide()
                    }

                    function create() {
                        createHinge(picked1, picked2, upper, lower)
                        resetEditor()
                        unhide()
                    }

                    onObjectChanged: {
                        pickedParents.model = []
                        resetEditor()
                        resetAll()

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
        animate: function() {
            hingeeditor.updateAnimation()
        }
    }
}
