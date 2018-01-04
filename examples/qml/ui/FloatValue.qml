import QtQuick 2.7

QtObject {
    id: main
    property string type: "float"
    property string name
    property real from: 0
    property real to: 100
    property QtObject target
    property real value

    onTargetChanged: {
        value = target[name]
        main.valueChanged.connect(setTargetValue)
    }

    function setTargetValue() {
        target[name] = value
    }
}
