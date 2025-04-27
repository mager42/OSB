#ifndef GSTREAMERPIPELINE_H
#define GSTREAMERPIPELINE_H

#include <QObject>

#include <gst/gst.h>

class GstreamerPipeline : public QObject
{
    Q_OBJECT
public:
    explicit GstreamerPipeline(uint width, uint height, QObject *parent = nullptr);
    ~GstreamerPipeline();

    Q_INVOKABLE void capturePicture(QString filename);
    static gboolean bus_call(GstBus *bus, GstMessage *msg, gpointer user_data);
    static void on_handoff(GstElement *fakesink, GstBuffer *buffer, GstPad *pad, gpointer user_data);

signals:

private:
    GstElement* m_pipeline;
    GstElement* m_fakeSink;
    QString m_lastPictureFilename;
    bool m_IsPictureCaptured;
    gulong handoff_handler_id;
    uint m_videoWidth;
    uint m_videoHeight;

    void setFakeSinkHandOffs();
    void unSetFakeSinkHandOffs();

};

#endif // GSTREAMERPIPELINE_H
