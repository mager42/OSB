import QtQuick
import QtQuick.Window
import QtQuick.Controls

Window {
    id: mainWindow
    width: 1024
    height: 720
    visible: true
    visibility: Qt.WindowFullScreen
    title: qsTr("OpenSnapBooth")

    Rectangle {
        id: mainCameraRectangle
        anchors.fill: parent
        color: "black"
    }
}
