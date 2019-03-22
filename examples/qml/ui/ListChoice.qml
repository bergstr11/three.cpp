import QtQuick 2.7

QtObject {
    property string type: "list"
    property string label: "List"
    property var names
    property var values
    property int selectedIndex: -1

    property string selectedValue

    onSelectedIndexChanged: selectedValue = !!values ? values[selectedIndex] : null

    function reset() {}
}
