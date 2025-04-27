#include "pictureutils.h"
#include <QDebug>
#include <QFile>
#include <QImage>
#include <QPainter>
#include <QProcess>

PictureUtils::PictureUtils(QObject *parent)
    : QObject{parent}
{
    
}

bool PictureUtils::deletePicture(const QString &filePath)
{
    QFile file(filePath);
    if (file.exists()) {
        if (file.remove()) {
            qDebug() << "File deleted successfully: " << filePath;
            return true;
        } else {
            qDebug() << "Error: Unable to delete file: " << filePath;
            return false;
        }
    } else {
        qDebug() << "File does not exist: " << filePath;
        return false;
    }
}

bool PictureUtils::IsPrinterAvailable()
{
    QString program = "lpstat";
    QStringList arguments;
    arguments << "-p" << "Canon_CP1000";

    QProcess *myProcess = new QProcess();
    myProcess->start(program, arguments);
    if( myProcess->waitForFinished() )
    {
        char response[128] = {0};
        if( myProcess->readLine(response, 128) != 1)
        {
            if( QString(response).split(" ").at(3).startsWith("idle") )
            {
                qDebug() << "The printer is availalbe and waiting for job";
                return true;
            }
            else
            {
                qDebug() << "The printer is available but not idle : " << response << QString(response).split(" ").at(3);
            }
        }
    }
    else
    {
        qDebug() << "The printer is not available or not idle";
    }
    return false;
}

bool PictureUtils::printPicture(const QString &filePath)
{
    if(IsPrinterAvailable())
    {
        QString program = "lp";
        QStringList arguments;
        arguments << "-d" << "Canon_CP1000"
                  << "-o" << "StpBorderless=true"
                  << "-o" << "fill"
                  << "-o" << "print-quality=5"
                  << filePath;

        QProcess *myProcess = new QProcess();
        myProcess->start(program, arguments);
        if( myProcess->waitForFinished() )
        {
            qDebug() << filePath << "sent to the printer succesfully";
            return true;
        }
        else
        {
            qDebug() << "Unable to send the file to the printer";
            char response[128] = {0};
            if( myProcess->read(response, 128) != 1)
            {
                qDebug() << "Error : " << response;
            }
        }
    }
    return false;
}

void PictureUtils::addLogoToPicture(const QString &imagePath,
                                    const QString &logoPath,
                                    const float size,
                                    const float posX,
                                    const float posY)
{
    QImage image(imagePath);
    if (image.isNull()) {
        qDebug() << "Error: Cannot save an empty image! (imagePath = " << imagePath << ")";
        return;
    }

    QImage logo(logoPath);
    if (logo.isNull()) {
        qDebug() << "Warning: Logo not found (" << logoPath << "), saving original image.";
        return;
    }

    QImage scaledLogo = logo.scaled(image.width() * size,
                                    image.width() * size,
                                    Qt::KeepAspectRatio,
                                    Qt::SmoothTransformation);

    // Create a copy of the original image
    QImage outputImage = image.copy();
    QPainter painter(&outputImage);
    painter.drawImage(posX * image.width(), posY * image.height(), scaledLogo);
    painter.end();

    if (outputImage.save(imagePath, "JPEG"))
    {
        qDebug() << "Saving Image (" << imagePath
                 << ") with a logo at position x:" << posX * image.width()
                 << " y:" << posY * image.height();
        emit pictureWithLogoSaved();
    }
    else
        qDebug() << "Unable to save modified image";
}
