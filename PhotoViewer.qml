import QtQuick
import QtQuick.Controls

Item {
    id: photoViewer
    visible: false

    property string source: ""
    signal deletionValidatedCancelled()
    signal waitForValidation()

    Rectangle {
        color: "black"
        anchors.fill: parent

        Image {
            width: parent.width - 15
            height: parent.height - 15
            anchors.centerIn: parent
            source: photoViewer.source
            fillMode: Image.PreserveAspectCrop
        }
    }

    Column {
        spacing: 10
        anchors.left: photoViewer.left
        anchors.bottom: photoViewer.bottom
        anchors.leftMargin: 10
        anchors.bottomMargin: parent.height / 2 - 100 +10

        CustomButton {
            id: printerButton
            source: "qrc:/icons/printer.png"

            onClicked: {
                var fileToPrint = photoViewer.source.replace(/^(file:\/{2})/,"");
                // unescape html codes like '%23' for '#'
                fileToPrint = decodeURIComponent(fileToPrint);
                pictureUtils.printPicture(fileToPrint)
                console.log("Trying to print the photo ", fileToPrint)
            }
        }

        CustomButton {
            id: deleteButton
            source: "qrc:/icons/delete.png"

            onClicked: {
                deleteDialog.visible = true
                photoViewer.waitForValidation()
            }
        }
    }

    ConfirmationDialog {
        id: deleteDialog
        anchors.centerIn: parent

        onConfirmed: {
            console.log("Deletion canceled")
            var fileToDelete = source.toString()
            source = ""
            fileToDelete = fileToDelete.replace(/^(file:\/{2})/,"");
            // unescape html codes like '%23' for '#'
            fileToDelete = decodeURIComponent(fileToDelete);
            if (pictureUtils.deletePicture(fileToDelete)) {
                console.log("File deleted:", fileToDelete);
            } else {
                console.log("Failed to delete file:", fileToDelete);
            }
            deleteDialog.visible = false
            photoViewer.deletionValidatedCancelled()
        }

        onCancelled: {
            deleteDialog.visible = false
            photoViewer.deletionValidatedCancelled()
        }
    }
}
