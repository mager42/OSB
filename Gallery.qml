import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Dialogs
import Qt.labs.folderlistmodel

Dialog {
    id: galleryWindow
    width: parent.width - 50
    height: parent.height - 50
    visible: false
    modal: true
    title: ""  // Remove header
    standardButtons: Dialog.NoButton  // Remove footer buttons
    closePolicy: Popup.NoAutoClose
    //standardButtons: Dialog.Close

    property int selectedIndex: -1

    background: Rectangle {
        color: "white"
        opacity: 0.6
        radius: 10
        border.color: "black"
        border.width: 2
    }

    CustomButton {
        source:"qrc:/icons/close.png"
        width: 60; height: width
        x: Screen.width - 105
        y: -20
        border.color: "black"
        border.width: 1
        radius: width/2

        onClicked: galleryWindow.close()
    }

    Column {
        anchors.fill: parent

        // Large Image Preview
        PhotoViewer {
            id: photoViewer
            anchors.top: parent.top
            width: parent.width * 3/4
            height: parent.height * 3/4
            x: (parent.width/2) - (width/2)
            visible: folderModel.count > 0
        }

        // FolderListModel to load images dynamically
        FolderListModel {
            id: folderModel
            folder: settingsManager.saveImageLocation
            nameFilters: ["*.jpg", "*.png", "*.jpeg", "*.bmp"] // Only load images
            showDirs: false
            showHidden: false

            onCountChanged: {  // When new images are loaded or images are deleted
                if(selectedIndex < folderModel.count){
                    photoViewer.source = folderModel.folder + "/" + folderModel.get(selectedIndex, "fileName")
                    imageList.positionViewAtIndex(selectedIndex, GridView.Contain)
                }
                else
                    selectLastImage()
            }
        }

        // GridView for Image Thumbnails
        ListView {
            id: imageList
            anchors.bottom: parent.bottom
            width: parent.width
            height: parent.height * 1/4 - 20
            clip: true
            model: folderModel
            orientation: ListView.Horizontal

            delegate: Item {
                width: imageList.width / 5.5
                height: imageList.height - 10

                Rectangle {
                    id: recta
                    width: parent.width - 5
                    height: parent.height - 5
                    border.color: selectedIndex === index ? "black" : "transparent"
                    border.width: 10
                    radius: border.width
                    color: "transparent"

                    Image {
                        source: folderModel.folder + "/" + fileName
                        width: parent.width - 10
                        height: parent.height - 10
                        anchors.centerIn: parent
                        fillMode: Image.PreserveAspectCrop
                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                selectedIndex = index
                                photoViewer.source = folderModel.folder + "/" + folderModel.get(index, "fileName")
                                imageList.positionViewAtIndex(index, GridView.Contain)
                            }
                        }
                    }
                }
            }
        }
    }
    // Select the last image when the dialog is shown
    onVisibleChanged: {
        if (visible) {
            selectLastImage()
        }
    }

    Component.onCompleted: selectLastImage()

    // Function to select the last image
    function selectLastImage() {
        if (folderModel.count > 0) {
            selectedIndex = folderModel.count - 1
            photoViewer.source = folderModel.folder + "/" + folderModel.get(folderModel.count - 1, "fileName")
            imageList.positionViewAtEnd()
        }
    }
}
