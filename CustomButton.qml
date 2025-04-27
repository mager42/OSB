import QtQuick

Rectangle {
    radius: 5
    width: 100
    height: 100
    color: "white"
    opacity: 0.9

    property string source: ""
    signal clicked()

    Image {
        id: imageButton
        anchors.fill: parent
        source: parent.source
        fillMode: Image.PreserveAspectFit
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            parent.clicked()
        }
        onPressed: parent.color = "grey"
        onReleased: parent.color = "white"
    }
}
