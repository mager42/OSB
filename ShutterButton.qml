import QtQuick
import QtQuick.Layouts

Item {
    signal shutterButtonClicked()

    Column {
        anchors.centerIn: parent

        Text {
            x: -(width / 2)
            anchors.bottom: camera.top
            text: qsTr("Toucher l'ecran pour prendre une photo")
            color: "white"
            opacity: 0.5
            font.family: "FontsFree-Net-Modern2"
            font.pixelSize: 82
            Layout.alignment: Qt.AlignCenter
        }

        Image {
            id:camera
            anchors.centerIn: parent
            opacity: 0.25
            source: "qrc:/icons/camera.png"
            width: 350
            height: 350
            fillMode: Image.PreserveAspectFit
        }
    }

    MouseArea {
        id: capturePhotoButton
        anchors.fill: parent
        onClicked: {
            shutterButtonClicked()
        }
    }
}
