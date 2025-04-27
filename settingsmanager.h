#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <QCameraDevice>
#include <QMediaDevices>
#include <QObject>
#include <QSettings>
#include <QStringList>

class SettingsManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QStringList availableCameras READ availableCameras NOTIFY availableCamerasChanged)
    Q_PROPERTY(QString selectedCameraDescription READ selectedCameraDescription WRITE
                   setSelectedCameraDescription NOTIFY selectedCameraDescriptionChanged)
    Q_PROPERTY(
        QCameraDevice selectedCameraDevice READ selectedCameraDevice NOTIFY selectedCameraChanged)
    Q_PROPERTY(QString saveImageLocation READ saveImageLocation WRITE setSaveImageLocation NOTIFY
                   saveImageLocationChanged)
    Q_PROPERTY(float previewDelay READ previewDelay WRITE setPreviewDelay NOTIFY previewDelayChanged)
    Q_PROPERTY(float captureDelay READ captureDelay WRITE setCaptureDelay NOTIFY captureDelayChanged)
    Q_PROPERTY(QString logoPath READ logoPath WRITE setLogoPath NOTIFY logoPathChanged)
    Q_PROPERTY(int logoPosX READ logoPosX WRITE setlogoPosX NOTIFY logoPosXChanged)
    Q_PROPERTY(int logoPosY READ logoPosY WRITE setlogoPosY NOTIFY logoPosYChanged)

public:
    explicit SettingsManager(QObject *parent = nullptr);

    QStringList availableCameras() const;
    QString selectedCameraDescription() const;
    void setSelectedCameraDescription(const QString &description);
    QCameraDevice selectedCameraDevice() const;

    QString saveImageLocation() const;
    void setSaveImageLocation(const QString &location);

    float previewDelay() const;
    void setPreviewDelay(float newPreviewDelay);

    float captureDelay() const;
    void setCaptureDelay(float newCaptureDelay);

    QString logoPath() const;
    void setLogoPath(const QString &newLogoPath);

    int logoPosX() const;
    void setlogoPosX(int newLogoPosX);

    int logoPosY() const;
    void setlogoPosY(int newLogoPosY);

signals:
    void availableCamerasChanged();
    void selectedCameraDescriptionChanged();
    void selectedCameraChanged();
    void saveImageLocationChanged();
    void previewDelayChanged();
    void captureDelayChanged();
    void logoPathChanged();
    void logoPosXChanged();
    void logoPosYChanged();

private:
    QSettings m_settings;
    QList<QCameraDevice> m_cameraList;
    void detectCameras();

    const QString m_selectedCameraDescription_title = "selectedCameraDescription";
    const QString m_saveImageLocation_title = "saveImageLocation";
    const QString m_previewDelay_title = "previewDelay";
    const QString m_captureDelay_title = "captureDelay";
    const QString m_logoPath_title = "logoPath";
    const QString m_logoPosX_title = "logoPosX";
    const QString m_logoPosY_title = "logoPosY";
};

#endif // SETTINGSMANAGER_H
