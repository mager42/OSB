#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QScreen>
#include <QQmlContext>
#include <QtMultimedia/QCamera>
#include <QtMultimedia/QMediaCaptureSession>
#include "pictureutils.h"
#include "settingsmanager.h"
#include "gstreamerpipeline.h"
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

    SettingsManager settingsManager;
    PictureUtils pictureUtils;
    engine.rootContext()->setContextProperty("pictureUtils", &pictureUtils);
    engine.rootContext()->setContextProperty("settingsManager", &settingsManager);

//    gst_init(nullptr, nullptr);
//    // Create a GStreamer pipeline using videotestsrc
//    GstElement* pipeline = gst_parse_launch("libcamerasrc ! video/x-raw,width=1920,height=1080,framerate=30/1 ! queue ! waylandsink fullscreen=true", nullptr);
//    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    GstreamerPipeline myPipeline(/*screen->size().width(), screen->size().height()*/2304, 1296);
    engine.rootContext()->setContextProperty("gstreamerPipeline", &myPipeline);

    //engine.loadFromModule("Qt68_HelloWorld_minimal", "Main");
    engine.load(url);

    return app.exec();
}
