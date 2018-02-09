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

    signal setValue

    onTargetChanged: {
        value = base = eval("target."+name)
        main.valueChanged.connect(setValue)
    }
}
