import QtQuick 2.7

QtObject {
    id: main
    property string type: "float"
    property string name
    property real base
    property real from: 0
    property real to: 100
    property QtObject target
    property real value

    function reset() {
        value = base = eval("target."+name)
    }
    onTargetChanged: {
        reset()
    }
}
