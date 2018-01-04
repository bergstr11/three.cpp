import QtQuick 2.7

QtObject {
    id: main
    property string type: "menuchoice"
    property string name
    property bool value: false

    signal selected

    onValueChanged: {
        if(value) selected();
    }
}
