import QtQuick 2.7

QtObject {
    id: main
    property string type: "bool"
    property string name
    property alias label: main.name
    property bool value

    function reset() {}
}
