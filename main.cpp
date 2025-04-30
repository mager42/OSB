#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QScreen>
#include <QQmlContext>
#include <QtMultimedia/QCamera>
#include <QtMultimedia/QMediaCaptureSession>
#include "pictureutils.h"
#include "settingsmanager.h"
#include "gstreamerpipeline.h"
#include "cups_printer.hpp"
#include <gst/gst.h>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QScreen *screen = QGuiApplication::primaryScreen();

    QQmlApplicationEngine engine;
    const QUrl url(u"qrc:/OpenSnapBooth/Main.qml"_qs);
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);

    CupsPrinter myPrinter;

    SettingsManager settingsManager;
    PictureUtils pictureUtils;
    engine.rootContext()->setContextProperty("pictureUtils", &pictureUtils);
    engine.rootContext()->setContextProperty("settingsManager", &settingsManager);
    engine.rootContext()->setContextProperty("printer", &myPrinter);

    GstreamerPipeline myPipeline(/*screen->size().width(), screen->size().height()*/2304, 1296);
    engine.rootContext()->setContextProperty("gstreamerPipeline", &myPipeline);

    //engine.loadFromModule("Qt68_HelloWorld_minimal", "Main");
    engine.load(url);

    return app.exec();
}
