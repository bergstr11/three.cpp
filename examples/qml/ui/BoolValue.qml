import QtQuick 2.7

QtObject {
    id: main
    property string type: "bool"
    property string label
    property string name
    property Item threeD
    property QtObject target
    property bool value

    function reset() {
        value = target[name]
    }

    function setTargetValue() {
        target[name] = value
        if(!!threeD) threeD.update()
    }
    
    onTargetChanged: {
        reset()
        main.valueChanged.connect(setTargetValue)
    }
}
