import QtQuick 2.15
import QtQuick.Controls 2.15

Popup {
    id: deleteDialog
    width: 640
    height: 200
    modal: true
    dim: true
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

    property alias message: messageText.text
    signal confirmed()
    signal cancelled()

    background: Rectangle {
        color: "#ffffff"
        radius: 10
        border.color: "black"
        border.width: 2
    }

    Column {
        anchors.centerIn: parent
        spacing: 15

        Text {
            id: messageText
            text: "Etes-vous sûre de vouloir supprimer cette photo?"
            font.pixelSize: 24
            color: "black"
            width: deleteDialog.width - 10
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WordWrap
        }

        Row {
            spacing: 20
            anchors.horizontalCenter: parent.horizontalCenter

            Button {
                text: "Supprimer"
                width: 140  // Ensure button has a fixed width
                height: 60  // Ensure button has a fixed height

                background: Rectangle {
                    color: "#999999"
                    radius: 5
                    width: parent.width  // Ensures Rectangle matches button size
                    height: parent.height
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
                    deleteDialog.confirmed()
                    deleteDialog.close()
                }
            }

            Button {
                text: "Annuler"
                width: 140
                height: 60
                font.pixelSize: 18
                background: Rectangle {
                    color: "#cccccc"
                    radius: 5
                }
                onClicked: {
                    deleteDialog.cancelled()
                    deleteDialog.close()
                }
            }
        }
    }
}
