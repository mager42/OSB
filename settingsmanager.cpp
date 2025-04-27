#include "settingsmanager.h"

#include <QDir>

SettingsManager::SettingsManager(QObject *parent)
    : QObject(parent)
    , m_settings("MALT_Location", "OpenSnapBooth")
{
}

// Get available camera descriptions
QStringList SettingsManager::availableCameras() const
{
    QStringList cameraNames;
    for (const QCameraDevice &camera : m_cameraList) {
        qDebug() << "camera identify : " << camera.description();
        cameraNames.append(camera.description()); // Store descriptions
    }
    cameraNames.append("/dev/media0");
    return cameraNames;
}

// Get stored camera description
QString SettingsManager::selectedCameraDescription() const
{
    return m_settings.value(m_selectedCameraDescription_title, "").toString();
}

// Set selected camera and store its description
void SettingsManager::setSelectedCameraDescription(const QString &description)
{
    if (description != selectedCameraDescription()) {
        m_settings.setValue(m_selectedCameraDescription_title, description);
        emit selectedCameraDescriptionChanged();
        emit selectedCameraChanged();
    }
}

// Get the selected camera as a `QCameraDevice`
QCameraDevice SettingsManager::selectedCameraDevice() const
{
    for (const QCameraDevice &camera : m_cameraList) {
        if (camera.description() == selectedCameraDescription()) {
            return camera; // Return the matched QCameraDevice
        }
    }
    return QCameraDevice(); // Return an empty device if not found
}

// Detect available cameras
void SettingsManager::detectCameras()
{
    m_cameraList = QMediaDevices::videoInputs(); // Store full QCameraDevice objects
    emit availableCamerasChanged();
}

QString SettingsManager::saveImageLocation() const
{
    return m_settings.value(m_saveImageLocation_title, QDir::homePath())
        .toString(); // Default: Home directory
}

void SettingsManager::setSaveImageLocation(const QString &location)
{
    if (location != saveImageLocation()) {
        m_settings.setValue(m_saveImageLocation_title, location);
        emit saveImageLocationChanged();
    }
}

float SettingsManager::previewDelay() const
{
    return m_settings.value(m_previewDelay_title, 5.0).toFloat();
}

void SettingsManager::setPreviewDelay(float newPreviewDelay)
{
    if (previewDelay() != newPreviewDelay) {
        m_settings.setValue(m_previewDelay_title, newPreviewDelay);
        emit previewDelayChanged();
    }
}

float SettingsManager::captureDelay() const
{
    return m_settings.value(m_captureDelay_title, 5.0).toFloat();
}

void SettingsManager::setCaptureDelay(float newCaptureDelay)
{
    if (captureDelay() != newCaptureDelay) {
        m_settings.setValue(m_captureDelay_title, newCaptureDelay);
        emit captureDelayChanged();
    }
}

QString SettingsManager::logoPath() const
{
    return m_settings.value(m_logoPath_title, "").toString();
}

void SettingsManager::setLogoPath(const QString &newLogoPath)
{
    if (logoPath() != newLogoPath) {
        m_settings.setValue(m_logoPath_title, newLogoPath);
        emit logoPathChanged();
    }
}

int SettingsManager::logoPosX() const
{
    return m_settings.value(m_logoPosX_title, 0).toInt();
}

void SettingsManager::setlogoPosX(int newLogoPosX)
{
    if (logoPosX() != newLogoPosX) {
        m_settings.setValue(m_logoPosX_title, newLogoPosX);
        emit logoPosXChanged();
    }
}

int SettingsManager::logoPosY() const
{
    return m_settings.value(m_logoPosY_title, 0).toInt();
}

void SettingsManager::setlogoPosY(int newLogoPosY)
{
    if (logoPosY() != newLogoPosY) {
        m_settings.setValue(m_logoPosY_title, newLogoPosY);
        emit logoPosYChanged();
    }
}
