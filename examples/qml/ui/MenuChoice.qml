import QtQuick 2.7

QtObject {
    id: main
    property string type: "menuchoice"
    property string name
    property color textColor: "black"
    property color color: "transparent"
    property color selectedColor: "darkgray"
    property bool value: false
    property bool isAction: false

    signal selected

    onValueChanged: {
        if(value) {
            selected();
            if(isAction) value = false
        }
    }

    function reset() {}
}
