import QtQuick

Item {
    property int remainingTime: 10  // Countdown time in seconds
    signal finish()

    Timer {
        id: countdownTimer
        interval: 1000  // 1 second
        repeat: true
        running: false

        property int counter: 0

        onTriggered: {
            if (counter > 1) {
                counter--
                timerText.text = counter
            } else {
                countdownTimer.stop()
                parent.visible = false
                finish()
            }
        }
    }
    Text {
        anchors.centerIn: parent
        id: timerText
        text: ""
        color: "white"
        font.pixelSize: 72
    }

    function start() {
        countdownTimer.counter = remainingTime
        timerText.text = countdownTimer.counter
        timerText.visible = true
        countdownTimer.start()
        visible = true
    }
}
