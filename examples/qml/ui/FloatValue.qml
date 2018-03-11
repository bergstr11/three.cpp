import QtQuick 2.7

QtObject {
    id: main
    property string type: "float"
    property string name
    property string label
    property real from: 0
    property real to: 100
    property QtObject target
    property real value

    function reset() {
        value = target[name]
    }

    function setTargetValue() {
        target[name] = value
    }

    onTargetChanged: {
        reset()
        main.valueChanged.connect(setTargetValue)
    }
}
