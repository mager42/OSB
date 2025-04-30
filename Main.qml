import QtQuick
import QtQuick.Controls
import QtQuick.Window
import QtMultimedia

import QtQuick.Controls.Material

Window {
    id: mainWindow
    visible: true
    title: qsTr("Hello World")
    visibility: Window.FullScreen
//    width: 1280
//    height:720
    color:"transparent"

    Shortcut {
        sequence: StandardKey.Quit
        context: Qt.ApplicationShortcut
        onActivated: Qt.quit()
    }

    Material.theme: Material.Dark
    Material.accent: Material.Red
    Material.primary: Material.Blue

    Settings {
        id: settingsWindow
        visible: false
        anchors.centerIn: parent
    }

    Gallery {
        id: galleryWindow
        visible: false
        anchors.centerIn: parent
    }

//    Item {
//        anchors.fill: parent
//        VideoOutput {
//            id: output
//            anchors.fill: parent
//        }
//        MediaDevices {
//            id: devices
//        }

//        Camera {
//            id: myCamera
//            cameraDevice: settingsManager.selectedCameraDevice

//            onErrorOccurred: (id, error, message) => {
//                console.log("Error on camera : ", message);
//            }
//        }

//        CaptureSession {
//            id: captureSession
//            videoOutput: output

//            camera: myCamera
//            imageCapture: ImageCapture {
//                id: imageCapture
//                onErrorOccurred: (id, error, message) => {
//                    console.log("Error capturing image:", message);
//                    console.log("State: ", myCamera.active)
//                    myCamera.setActive(false)
//                                     myCamera.setActive(true)
//                    myCamera.start()
//                }
//                onImageSaved:(id, filename) => {
//                    console.log("Image Saved - filename: ", filename);
//                    var logoPath = settingsManager.logoPath.replace(/^(file:\/{2})/,"");
//                    // unescape html codes like '%23' for '#'
//                    logoPath = decodeURIComponent(logoPath);
//                    var scaledLogoPosX = settingsManager.logoPosX / Screen.width;
//                    var scaledLogoPosY = settingsManager.logoPosY / Screen.height;
//                    pictureUtils.addLogoToPicture(filename, logoPath, logo.width / Screen.width, scaledLogoPosX, scaledLogoPosY);
//                    previewer.source = "file://" + filename;
//                }
//            }
//        }
//    }

    ShutterButton {
        anchors.centerIn: parent
        anchors.fill: parent

        onShutterButtonClicked: {
            countDown.start()
            galleryButton.enabled = false
        }
    }

    CountDown {
        id: countDown
        anchors.centerIn: parent
        visible: false
        remainingTime: settingsManager.captureDelay

        onFinish: {
            flash.show()
            galleryButton.enabled = true
        }
    }

    CustomButton {
        id:galleryButton
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.leftMargin: 10
        anchors.bottomMargin: 10
        source: "qrc:/icons/gallery.png"

        onClicked: galleryWindow.visible = true
    }

    Image {
        id: logo
        x: settingsManager.logoPosX
        y: settingsManager.logoPosY
        source: settingsManager.logoPath === "" ? "qrc:/icons/settings.png" : settingsManager.logoPath
        fillMode: Image.PreserveAspectFit
        width: 250
        height: 250

        MouseArea {
            anchors.fill: parent
            pressAndHoldInterval: 5000
            onPressAndHold: settingsWindow.visible = true
        }
    }

    Timer {
        id: previewTimer
        interval: settingsManager.previewDelay*1000
        running: false
        repeat: false
        triggeredOnStart : true

        onTriggered: () => {
            previewer.visible = !previewer.visible
        }
    }

    PhotoViewer {
        id: previewer
        anchors.centerIn: parent
        width: parent.width - 50
        height: parent.height - (50/1.77777)
        onDeletionValidatedCancelled: {
            previewer.visible = false
            previewTimer.stop()
        }
        onWaitForValidation: {
            previewTimer.stop()
        }
    }

    Flash {
        id: flash
        anchors.centerIn: parent
        anchors.fill: parent

        onFlashDisplayed: {
            takeAPicture()
            //previewTimer.start()
        }
    }

    // Timer for Creating delay
    Timer {
        id: delayTimerForLogo
        interval: 500
        running: false
        repeat: false
        triggeredOnStart: false

        property string fileLocation : ""
        property string logoLocation : ""

        property real scaledLogoPosX : settingsManager.logoPosX / Screen.width
        property real scaledLogoPosY : settingsManager.logoPosY / Screen.height

        onTriggered: () => {
            console.log("Delay logo overlay for file", fileLocation);
            pictureUtils.addLogoToPicture(fileLocation, logoLocation, logo.width / Screen.width, scaledLogoPosX, scaledLogoPosY);
            previewer.source = "file://" + fileLocation;
        }
    }

    Connections {
        target: pictureUtils
        function onPictureWithLogoSaved() {
            console.log("onPictureWithLogoSaved")
            previewTimer.start()
        }
    }

    ErrorDialog {
        id: errorDialog
        visible: false
        anchors.centerIn: parent

        onClosed: () => {
            printer.clearPrinterError()
        }
    }

    Connections {
        target: printer
        function onPrinterRaisedError() {
            console.log("onPrinterRaisedError")
            errorDialog.message = printer.printerErrorMessage
            errorDialog.visible = true
        }
    }

    function takeAPicture() {
        console.log("Take a Picture");
        var savePath = settingsManager.saveImageLocation.toString() + "/" + Qt.formatDateTime(new Date(), "yyMMdd_hhmms") + ".jpg";
        // remove prefixed "file://"
        savePath = savePath.replace(/^(file:\/{2})/,"");
        // unescape html codes like '%23' for '#'
        savePath = decodeURIComponent(savePath);
        console.log("Should take a picture and save it here :", savePath);
        gstreamerPipeline.capturePicture(savePath);
        var logoPath = settingsManager.logoPath.replace(/^(file:\/{2})/,"");
        // unescape html codes like '%23' for '#'
        logoPath = decodeURIComponent(logoPath);
        var scaledLogoPosX = settingsManager.logoPosX / Screen.width;
        var scaledLogoPosY = settingsManager.logoPosY / Screen.height;

        delayTimerForLogo.logoLocation = logoPath
        delayTimerForLogo.fileLocation = savePath
        delayTimerForLogo.start()
    }
}
