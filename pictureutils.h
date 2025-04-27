#ifndef PICTUREUTILS_H
#define PICTUREUTILS_H

#include <QObject>

class PictureUtils : public QObject
{
    Q_OBJECT
public:
    explicit PictureUtils(QObject *parent = nullptr);

    Q_INVOKABLE static bool deletePicture(const QString &filePath);
    Q_INVOKABLE static bool IsPrinterAvailable();
    Q_INVOKABLE static bool printPicture(const QString &filePath);
    Q_INVOKABLE void addLogoToPicture(const QString &imagePath,
                                             const QString &logoPath,
                                             const float size,
                                             const float posX,
                                             const float posY);

signals:
    void pictureDeleted();
    void pictureWithLogoSaved();
};

#endif // PICTUREUTILS_H
