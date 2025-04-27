import QtQuick

Rectangle {
    id: screenflash
    visible: true
    opacity: 0
    color: "#FFFCFF"

    signal flashDisplayed()

    // Fade In Animation
    SequentialAnimation {
        id: fadeInAnimation
        PropertyAnimation {
            target: screenflash
            property: "opacity"
            from: 0
            to: 1
            duration: 100
            easing.type: Easing.InOutQuad
        }
    }

    // Fade Out Animation
    SequentialAnimation {
        id: fadeOutAnimation
        PropertyAnimation {
            target: screenflash
            property: "opacity"
            from: 1
            to: 0
            duration: 500
            easing.type: Easing.InOutQuad
        }
    }

    Timer {
        id: hideFlashTimer
        interval: 1000
        running: false
        repeat: false

        onTriggered: () => {
            flashDisplayed()
            Qt.callLater(function() {
                    hide()
                }, 500)
        }
    }

    // Appear animation
    function show() {
        fadeInAnimation.restart()
        hideFlashTimer.start()
    }

    // Disappear animation
    function hide() {
        fadeOutAnimation.restart()
    }
}
