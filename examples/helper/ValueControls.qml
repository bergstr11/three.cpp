import QtQuick 2.7
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.1

Rectangle {
    id: main
    color: "darkgray"

    property string title;
    property Item scene
    default property list<FloatValue> properties

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
                }
                Slider {
                    id: slider
                    width: parent.width - 70
                    from: from
                    to: to
                    value: target.value

                    onValueChanged: {
                        target.value = value
                        scene.update()
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
            prev = float_control.createObject(main, {"anchors.top": prev.bottom,
                                                  name: prop.name, from: prop.from, to: prop.to, target: prop})
            height += prev.implicitHeight
        }
        main.height = height
    }
}
