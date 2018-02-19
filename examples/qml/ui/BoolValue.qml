import QtQuick 2.7

QtObject {
    id: main
    property string type: "bool"
    property string name
    property QtObject target
    property bool value

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
