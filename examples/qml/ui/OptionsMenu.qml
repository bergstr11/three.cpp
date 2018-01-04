import QtQuick 2.7
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.1

Rectangle {
    id: main
    color: "darkgray"

    property string title;
    property color labelColor: "white"
    property Item threeD
    default property list<QtObject> properties

    property list<MenuChoice> menuChoices

    Item {
        id: titleRow
        anchors.top:parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: 30

        Label {
            anchors.centerIn: parent
            text: title
            font.bold: true
        }
    }

    Component {
        id: bool_control
        Item {
            property Item prev
            anchors.top: prev.top
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.leftMargin: 10
            anchors.rightMargin: 10
            implicitHeight: 40

            property string name
            property QtObject target

            Row {
                anchors.fill: parent
                spacing: 10

                Label {
                    height: parent.height
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignRight
                    width: 80
                    text: name
                    color: labelColor
                    font.bold: true
                }
                CheckBox {
                    width: parent.width - 90
                    checked: target.value

                    onCheckedChanged: {
                        target.value = checked
                    }
                }
            }
        }
    }

    Component {
        id: menu_choice
        Rectangle {
            property Item prev
            anchors.top: prev.top
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.leftMargin: 10
            anchors.rightMargin: 10
            implicitHeight: 30

            property string name
            property QtObject target

            color: target.value ? "lightgray" : "transparent"

            Label {
                anchors.fill: parent
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                text: name
                color: target.value ? "green" : labelColor
                font.bold: true

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        for(var i=0; i<menuChoices.length; i++)
                            menuChoices[i].value = (menuChoices[i] === target)
                        //target.selected()
                    }
                }
            }
        }
    }

    Component {
        id: float_control
        Item {
            property Item prev
            anchors.top: prev.top
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.leftMargin: 10
            anchors.rightMargin: 10
            implicitHeight: 40

            property string name
            property alias from: slider.from
            property alias to: slider.to
            property QtObject target

            Row {
                anchors.fill: parent
                spacing: 10

                Label {
                    height: parent.height
                    verticalAlignment: Text.AlignVCenter
                    width: 60
                    text: name
                    font.bold: true
                    color: labelColor
                }
                Slider {
                    id: slider
                    width: parent.width - 70
                    from: from
                    to: to
                    value: target.value

                    onValueChanged: {
                        target.value = value
                        threeD.update()
                    }
                    handle: Rectangle {
                            x: slider.leftPadding + slider.visualPosition * (slider.availableWidth - width)
                            y: slider.topPadding + slider.availableHeight / 2 - height / 2
                            implicitWidth: 30
                            implicitHeight: 30
                            radius: 15
                            color: slider.pressed ? "#f0f0f0" : "#f6f6f6"
                            border.color: "#bdbebf"

                            Label {
                                anchors.centerIn: parent
                                text: slider.value.toFixed(2)
                            }
                        }
                }
            }
        }
    }

    Component.onCompleted: {
        var height = 10 + titleRow.height
        var prev = titleRow

        for(var index=0; index < properties.length; index++) {
            var prop = properties[index]
            if(prop.type === "float")
                prev = float_control.createObject(main, {"anchors.top": prev.bottom,
                                                      name: prop.name, from: prop.from, to: prop.to, target: prop})
            else if(prop.type === "bool")
                prev = bool_control.createObject(main, {"anchors.top": prev.bottom, name: prop.name, target: prop})
            else if(prop.type === "menuchoice") {
                prev = menu_choice.createObject(main, {"anchors.top": prev.bottom, name: prop.name, target: prop})
                menuChoices.push(prop)
            }
            height += prev.implicitHeight
        }
        main.height = height
    }
}
