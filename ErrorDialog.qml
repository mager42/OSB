import QtQuick 2.15
import QtQuick.Controls 2.15

Popup {
    id: errorDialog
    width: 640
    height: 200
    modal: true
    dim: true
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

    property alias message: messageText.text
    signal acknowledged()

    background: Rectangle {
        color: "#ffffff"
        radius: 10
        border.color: "black"
        border.width: 2
    }

    Column {
        anchors.centerIn: parent
        spacing: 15
        width: implicitWidth

        Text {
            id: messageText
            text: "ERROR"
            font.pixelSize: 24
            color: "black"
            width: errorDialog.width - 10
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WordWrap
        }

        Button {
            text: "OK"
            width: 140  // Ensure button has a fixed width
            height: 60  // Ensure button has a fixed height
            anchors.horizontalCenter: parent.horizontalCenter

            background: Rectangle {
                color: "#999999"
                radius: 5
                anchors.fill: parent
            }

            contentItem: Text {
                text: parent.text
                color: "white"
                font.bold: true
                font.pixelSize: 18  // Adjust text size
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                width: parent.width  // Ensure text takes full width
                height: parent.height
            }

            onClicked: {
                errorDialog.acknowledged()
                errorDialog.close()
            }
        }
    }
}
