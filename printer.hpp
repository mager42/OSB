#ifndef PRINTER_HPP
#define PRINTER_HPP

#include <string>
#include <QObject>
#include <QString>

class Printer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool isPrinterReady READ isPrinterReady NOTIFY printerStatusChanged)
    Q_PROPERTY(QString printerErrorMessage READ getPrinterErrorMessage NOTIFY printerRaisedError)

public:
    Printer(QObject *parent = nullptr) 
        :QObject{parent}{};
    ~Printer() = default;

    virtual bool isPrinterReady() = 0;
    virtual QString getPrinterErrorMessage() = 0;
    Q_INVOKABLE virtual void printFile(const QString &filePath) = 0;
    Q_INVOKABLE virtual void clearPrinterError() = 0;

signals:
    void printerRaisedError();
    void printerStatusChanged();
};

#endif // PRINTER_HPP