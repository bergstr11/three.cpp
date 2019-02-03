import QtQuick 2.7
import QtQuick.Controls 2.5
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
                    var obj = pickedParents.currentIndex === 0 ?
                        dataRow.picked : dataRow.picked.parentObject(pickedParents.model[pickedParents.currentIndex])
                    obj.visible = false
                    hingeeditor.hidden.push(obj)
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
                    var obj = pickedParents.currentIndex === 0 ?
                        dataRow.picked : dataRow.picked.parentObject(pickedParents.model[pickedParents.currentIndex])

                    var wp = obj.worldPosition()

                    if(!!hingeeditor.picked1) {
                        hingeeditor.picked2 = obj
                        textO2.text = obj.name + "(" + wp.x.toFixed(2) + ":" + wp.y.toFixed(2) + ":" + wp.z.toFixed(2) + ")"
                    }
                    else {
                        hingeeditor.picked1 = obj
                        textO1.text = obj.name + "(" + wp.x.toFixed(2) + ":" + wp.y.toFixed(2) + ":" + wp.z.toFixed(2) + ")"
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
            enabled: dynamics.hingeCount > 0
            text: "Animate"

            property bool running: false

            onClicked: {
                running = !running
                threeD.runAnimation(running)

                if(running) {
                    text = "Stop"
                }
                else {
                    text = "Animate"
                }
            }
        }
        Button {
            id: ffButton
            enabled: dynamics.hingeCount > 0 && !runButton.running
            text: "Open Doors"

            property bool forward: true

            onClicked: {
                forward = !forward

                if(forward) {
                    text = "Open Doors"
                    dynamics.fastforward("door", -1.0)
                }
                else {
                    text = "Close Doors"
                    dynamics.fastforward("door", 1.0)
                }
                threeD.update()
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
                    dynamics.load(fileDialog.fileUrl, hingeeditor.object)
                }
                fileDialog.visible = true
            }
        }
        Button {
            id: saveButton
            text: "Save"
            enabled: dynamics.hingeCount > 0
            onClicked: {
                fileDialog.title = "Save hinge definition to file"
                fileDialog.selectExisting = false
                fileDialog.acceptedFunc = function() {dynamics.save(fileDialog.fileUrl)}
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

        property QtObject target
        property string colorName: "color"

        onVisibleChanged: {
            if(visible) {
                color = target[colorName]
            }
        }
        onAccepted: {
            if(target) {
                target[colorName] = colorDialog.color
                threeD.update()
            }
        }
    }

    OptionsMenu {
        id: lightControls
        title: "Lighting"
        anchors.top: replBtn.bottom
        anchors.left: parent.left
        width: 300
        color: "transparent"
        z: 2
        threeD: threeD

        BoolChoice {
            name: "pointLight"
            value: false
            onValueChanged: {
                pointLight1.visible = value
                pointLight2.visible = value
                threeD.update()
            }
        }
        BoolValue {
            label: "hemisphereLight"
            name: "visible"
            target: hemisphereLight
        }
        BoolValue {
            label: "directionalLight"
            name: "visible"
            target: directionalLight
        }
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
                colorDialog.target = hemisphereLight
                colorDialog.colorName = "skyColor"
                colorDialog.visible = true
            }
        }
        MenuChoice {
            name: "directional color"
            isAction: true
            onSelected: {
                colorDialog.target = directionalLight
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
        ListChoice {
            id: pickOpChoice
            label: "PickOp:"
            values: ["control", "select", "mark", "material info", "enhance paint", "enhance metal", "set color"]
            selectedIndex: 0
        }
    }
    ColumnLayout {
        anchors.top: objectControls.bottom
        anchors.right: parent.right
        anchors.margins: 10
        spacing: 10
        z: 2

        ComboBox {
            id: hingeSelector
            model: dynamics.hingeNames
            Layout.fillWidth: true
        }

        Button {
            text: "Delete"
            Layout.fillWidth: true
            enabled: hingeSelector.currentIndex >= 0
            onClicked: {
                dynamics.deleteHinge(hingeSelector.model[hingeSelector.currentIndex])
                threeD.update()
            }
        }
        Button {
            enabled: hingeeditor.doorComplete
            text: "Create door"
            Layout.fillWidth: true

            onClicked: hingeeditor.create("door")
        }
        Button {
            enabled: hingeeditor.propellerComplete
            text: "Create propeller"
            Layout.fillWidth: true

            onClicked: hingeeditor.create("propeller")
        }
        Button {
            enabled: hingeeditor.wheelsComplete
            text: "Create wheels"
            Layout.fillWidth: true

            onClicked: hingeeditor.create("wheels")
        }
        Button {
            text: "Reset"
            enabled: hingeeditor.dataStarted
            Layout.fillWidth: true

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
        id: replBtn
        text: "Set"
        z: 2
        anchors.top: parent.top
        anchors.left: parent.left
        width: 55
        height: 35
        onClicked: {
            fileDialog.title = "Please choose a model file"
            fileDialog.selectExisting = true
            hingeeditor.operation = ModelRef.Replace
            fileDialog.acceptedFunc = function() {threeDModel.file = fileDialog.fileUrl}
            fileDialog.visible = true
        }
    }
    Button {
        text: "Add"
        z: 2
        width: 55
        height: 35
        anchors.top: parent.top
        anchors.left: replBtn.right
        anchors.leftMargin: 3
        onClicked: {
            fileDialog.title = "Please choose a model file"
            fileDialog.selectExisting = true
            //fileDialog.selectMultiple = true
            hingeeditor.operation = ModelRef.Append
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

    Popup {
        id: infoPopup
        width: 400
        height: 600
        modal: true
        focus: true
        closePolicy: Popup.CloseOnEscape

        property alias text: popupText.text

        ScrollView {
            anchors.fill: parent
            clip: true
            Text {
                id: popupText
                wrapMode: Text.Wrap

                MouseArea {
                    anchors.fill: parent
                    onClicked: infoPopup.close()
                }
            }
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
            enabled: pickOpChoice.selectedIndex > 0
            objects: [scene]

            onObjectPicked: {
                var is = intersect(0)
                pickedParents.model = objectPicker.pickedParents(0)
                dataRow.picked = is.object

                if(pickOpChoice.selectedValue == "mark") {
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
                else if(pickOpChoice.selectedValue == "enhance paint") {

                    paintMaterial.color = is.object.material.color
                    paintMaterial.emissive = is.object.material.emissive
                    is.object.material = paintMaterial.cloned()
                    threeD.update()
                }
                else if(pickOpChoice.selectedValue == "enhance metal") {

                    is.object.material = metalMaterial.cloned()
                    threeD.update()
                }
                else if(pickOpChoice.selectedValue == "material info") {

                    infoPopup.text = is.object.material.getInfo()
                    console.log(infoPopup.text)

                    infoPopup.x = objectPicker.mouseX();
                    infoPopup.y = Math.min(objectPicker.mouseY(), threeD.height - infoPopup.height);
                    infoPopup.open()
                }
                else if(pickOpChoice.selectedValue == "set color") {

                    colorDialog.target = is.object.material
                    colorDialog.colorName = "color"
                    colorDialog.visible = true
                }
            }
        }

        Dynamics {id: dynamics}

        //enhanced paint surface
        MeshPhysicalMaterial {
            id: paintMaterial
            emissive: "white"
            metalness: 0
			roughness: 0
			clearCoat: 1.0
			clearCoatRoughness: 0.0
			reflectivity: 1.0

			envMap: ImageTexture {
                format: Texture.RGBA
                mapping: Texture.SphericalReflection
                flipY: false
                type: Texture.UnsignedByte
                image: ":/metal4.jpg"
            }
        }

        //enhanced metallic effect (wheels etc.)
        MeshPhysicalMaterial {
            id: metalMaterial
            color: "white"
            metalness: 1.0
			roughness: 0
			clearCoat: 0
			clearCoatRoughness: 0
			reflectivity: 1.0

			envMap: ImageTexture {
                format: Texture.RGBA
                mapping: Texture.SphericalReflection
                flipY: false
                type: Texture.UnsignedByte
                image: ":/metal5.jpg"
            }
        }

        Scene {
            id: scene

            HemisphereLight {
                id: hemisphereLight
                name: "hemisphereLight"
                skyColor: "#fcfade"
                groundColor: Qt.hsla(0.095, 1, 0.75, 1)
                intensity: 0.6
                position.y: 250
                onVisibleChanged: threeD.update()
            }

            ConvexHull {
                id: convexHull
                visible: false

                material: MeshBasicMaterial {color: "green"; wireframe: true}

                HingeEditorModelRef {
                    id: hingeeditor
                    model: threeDModel
                    name: "threeD_model"
                    type: ModelRef.Node
                    dynamics: dynamics

                    property var hidden: []
                    property var picked1: null
                    property var picked2: null
                    property vector3d upper: "0,0,0"
                    property vector3d lower: "0,0,0"
                    property bool upperSet: false
                    property bool lowerSet: false

                    property bool dataStarted: !!picked1 || upperSet || lowerSet
                    property bool doorComplete: !!picked1 && !!picked2 && upperSet && lowerSet
                    property bool wheelsComplete: !!picked1 && !!picked2 && upperSet
                    property bool propellerComplete: !!picked1 && upperSet

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

                    function create(what) {
                        if(what == "door")
                            dynamics.createDoorHinge("door", picked1, picked2, upper, lower)
                        else if(what == "propeller")
                            dynamics.createPropellerHinge("propeller", picked1, upper)
                        else if(what == "wheels")
                            dynamics.createWheelHinge("wheels", picked1, picked2, upper)
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
                fov: 30
                aspect: threeD.width / threeD.height
                near: 1
                far: 100000
                position: "0,0,300"

                target: scene.position

                controller: OrbitController {
                    id: orbitController
                    maxPolarAngle: Math.PI;
                    enablePan: true
                    enabled: pickOpChoice.selectedIndex === 0
                    rotateCursor: Qt.ClosedHandCursor
                    panCursor: Qt.SizeAllCursor
                }

                PointLight {
                    id: pointLight1
                    color: "#ffffff";
                    intensity: 0.5
                    position: "-1525,0,600"
                    visible: false
                }
                PointLight {
                    id: pointLight2
                    color: "#ffffff"
                    intensity: 0.75
                    position: "1345,1385,600"
                    visible: false
                }
                DirectionalLight {
                    id: directionalLight
                    color: Qt.hsla(0.1, 1, 0.95, 1)
                    position: Qt.vector3d(-1.3,1.75,1).times(130)
                    intensity: 0.5
                    onVisibleChanged: threeD.update()
                }
            }
        }
        animate: function() {
            dynamics.update()
        }
    }
}
