import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Dialogs

Dialog {
    readonly property int titleWidth: 150
    readonly property int fieldWidth: 400
    readonly property real itemSpacing: 10
    readonly property int buttonWidth: 100

    id: settingsWindow
    visible: false
    width: titleWidth + itemSpacing + fieldWidth + itemSpacing + buttonWidth
    height: 480

    title: "Settings Manager"
    //flags: ~Qt.WindowSystemMenuHint | Qt.Dialog
    modal: true
    closePolicy: Popup.NoAutoClose
    standardButtons: Dialog.Ok | Dialog.Cancel

    Column {
        anchors.centerIn: parent
        spacing: itemSpacing

        // Video Input Device
        Row {
            spacing: itemSpacing

            Text {
                text: "Video Input Device:"
                width: titleWidth
            }
            ComboBox  {
                id: cameraComboBox
                width: fieldWidth
                model: settingsManager.availableCameras  // Show descriptions
                currentIndex: settingsManager.availableCameras.indexOf(settingsManager.selectedCameraDescription)
            }
        }
        // Save Location
        Row {
            spacing: itemSpacing
            Text {
                text: "Save Location:"
                width: titleWidth
            }
            TextField {
                id: textfieldFolder
                readOnly: true
                width: fieldWidth
                text: settingsManager.saveImageLocation
            }
            Button {
                text: "Select..."
                onClicked: folderDialog.open()
            }

            FolderDialog {
                id: folderDialog
                title: "Select a Folder"
                currentFolder: settingsManager.saveImageLocation
                onAccepted: {
                    //folderDialog.currentFolder = folderDialog.selectedFolder
                    textfieldFolder.text = folderDialog.selectedFolder
                    console.log("nouveau folder set to ", folderDialog.selectedFolder)
                }
            }
        }
        // Preview Delay
        Row {
            spacing: itemSpacing
            Text {
                text: "Preview Delay"
                width: titleWidth
            }
            DoubleSpinBox {
                id: previewDelay
                value: decimalToInt(settingsManager.previewDelay)
                from: 0
                to: decimalToInt(30)

                function onRejected() {
                    value = decimalToInt(settingsManager.previewDelay)
                }
            }
        }
        // Capture Delay
        Row {
            spacing: itemSpacing
            Text {
                text: "Capture Delay"
                width: titleWidth
            }
            DoubleSpinBox {
                id: captureDelay
                value: decimalToInt(settingsManager.captureDelay)
                from: 0
                to: decimalToInt(30)

                function onRejected() {
                    value = decimalToInt(settingsManager.captureDelay)
                }
            }
        }
        // Logo Location
        Row {
            spacing: itemSpacing
            Text {
                text: "Icon Location:"
                width: titleWidth
            }
            TextField {
                id: textLogoLocation
                readOnly: false
                width: fieldWidth
                text: settingsManager.logoPath
            }
            Button {
                text: "Select..."
                onClicked: fileDialog.open()
            }

            FileDialog {
                id: fileDialog
                title: "Select an image"
                currentFolder: getPathWithoutFilename(settingsManager.logoPath)
                onAccepted: {
                    textLogoLocation.text = fileDialog.selectedFile
                    console.log("nouveau file set to ", fileDialog.selectedFile)
                }

                function getPathWithoutFilename(filePath) {
                    // Find the last occurrence of "/" in the path
                    let lastSlashIndex = filePath.lastIndexOf("/");
                    if (lastSlashIndex !== -1) {
                        return filePath.substring(0, lastSlashIndex + 1);
                    }
                    return filePath; // If no slash is found, return the original string
                }
            }
        }
        // Logo Position
        Row {
            spacing: itemSpacing
            Text {
                text: "Logo Position - X: "
                width: titleWidth
            }
            SpinBox {
                id: posX
                value: settingsManager.logoPosX
                from: 0
                to: Screen.width
                stepSize: 10

                onValueChanged: {
                    settingsManager.logoPosX = posX.value
                }
            }
            Text {
                text: " Y: "
                width: titleWidth
            }
            SpinBox {
                id: posY
                value: settingsManager.logoPosY
                from: 0
                to: Screen.height
                stepSize: 10

                onValueChanged: {
                    settingsManager.logoPosY = posY.value
                }
            }
        }
    }

    onAccepted: {
        settingsManager.selectedCameraDescription = cameraComboBox.currentText
        settingsManager.saveImageLocation = textfieldFolder.text
        settingsManager.previewDelay = previewDelay.realValue
        settingsManager.captureDelay = captureDelay.realValue
        settingsManager.logoPath = textLogoLocation.text
        settingsManager.logoPosX = posX.value
        settingsManager.logoPosY = posY.value
        settingsWindow.visible = false
    }

    onRejected: {
        previewDelay.onRejected()
        captureDelay.onRejected()
        textfieldFolder.text = settingsManager.saveImageLocation
        textLogoLocation.text = settingsManager.logoPath
        posX.value = settingsManager.logoPosX
        posY.value = settingsManager.logoPosY
    }
}
