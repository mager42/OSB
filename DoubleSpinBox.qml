import QtQuick
import QtQuick.Controls

SpinBox {
    from: 0
    to: decimalToInt(30)
    stepSize: decimalFactor
    editable: true

    property int decimals: 2
    property real realValue: value / decimalFactor
    readonly property int decimalFactor: Math.pow(10, decimals)

    function decimalToInt(decimal) {
        return decimal * decimalFactor
    }

    validator: DoubleValidator {
        bottom: Math.min(from, to)
        top:  Math.max(from, to)
        decimals: decimals
        notation: DoubleValidator.StandardNotation
    }

    textFromValue: function(value, locale) {
        return Number(value / decimalFactor).toLocaleString(locale, 'f', decimals)
    }

    valueFromText: function(text, locale) {
        return Math.round(Number.fromLocaleString(locale, text) * decimalFactor)
    }
}
