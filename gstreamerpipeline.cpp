#include "gstreamerpipeline.h"
#include <QDebug>
#include <QImage>
#include <QDateTime>

GstreamerPipeline::GstreamerPipeline(uint width, uint height, QObject *parent)
    : QObject{parent}
    , m_pipeline(nullptr)
    , m_fakeSink(nullptr)
    , m_lastPictureFilename()
    , m_videoWidth(width)
    , m_videoHeight(height)
    , m_IsPictureCaptured(false)
{
    gst_init(nullptr, nullptr);

    m_pipeline = gst_pipeline_new("SnapBoothPipeline");
    if( !m_pipeline )
    {
        qDebug() << "Unable to create pipeline instance, exit application";
        exit(-1);
    }
    GstElement* libcameraSrc = gst_element_factory_make("libcamerasrc", "source");
    GstElement* waylandSink = gst_element_factory_make("waylandsink", "waylandSink");
    m_fakeSink = gst_element_factory_make("fakesink", "fakeSink");
    GstElement* capsfilter = gst_element_factory_make("capsfilter", "capsfilter");
    GstElement* tee = gst_element_factory_make("tee", "tee");
    GstElement* queue1 = gst_element_factory_make("queue", "queue1");
    GstElement* queue2 = gst_element_factory_make("queue", "queue2");

    if (!libcameraSrc || !capsfilter || !tee || !queue1 || !waylandSink || !queue2 || !m_fakeSink)
    {
        qDebug() << "Failed to create elements. Ensure all plugins are available";
        gst_object_unref(m_pipeline);
        exit(-1);
    }

    // Set capabilities for capsfilter
    GstCaps* caps = gst_caps_new_simple("video/x-raw",
                               "width", G_TYPE_INT, m_videoWidth,
                               "height", G_TYPE_INT, m_videoHeight,
                               "framerate", GST_TYPE_FRACTION, 30, 1,
                               NULL);
    g_object_set(capsfilter, "caps", caps, NULL);
    gst_caps_unref(caps);

    g_object_set(libcameraSrc, "af-mode", 2, NULL);

    g_object_set(waylandSink, "fullscreen", TRUE, "rotate-method", 4, NULL);

    g_object_set(m_fakeSink, "sync", TRUE, "signal-handoffs", TRUE, NULL);
    //g_signal_connect(m_fakeSink, "handoff", G_CALLBACK(on_handoff), this);

    // Add elements to the pipeline
    gst_bin_add_many(GST_BIN(m_pipeline),
                     libcameraSrc, capsfilter, tee,
                     queue1, waylandSink,
                     queue2, m_fakeSink,
                     NULL);

    // Link elements: source ? capsfilter ? tee
    if (!gst_element_link_many(libcameraSrc, capsfilter, tee, NULL))
    {
        qDebug() << "Failed to link source, capsfilter, and tee.";
        gst_object_unref(m_pipeline);
        exit(-1);
    }

    // Link tee branches: tee ? queue1 ? waylandsink
    if (!gst_element_link_many(tee, queue1, waylandSink, NULL)) {
        qDebug() << "Failed to link first tee branch.";
        gst_object_unref(m_pipeline);
        exit(-1);
    }

    // Link second tee branch: tee ? queue2 ? appsink
    if (!gst_element_link_many(tee, queue2, m_fakeSink, NULL)) {
        qDebug() << "Failed to link second tee branch.";
        gst_object_unref(m_pipeline);
        exit(-1);
    }

    // Get the bus and add a watch with the callback
    GstBus* bus = gst_element_get_bus(m_pipeline);
    gst_bus_add_watch(bus, GstreamerPipeline::bus_call, m_pipeline);

    // Start the pipeline
    gst_element_set_state(m_pipeline, GST_STATE_PLAYING);
}

GstreamerPipeline::~GstreamerPipeline()
{
    gst_element_set_state(m_pipeline, GST_STATE_NULL);
    gst_object_unref(m_pipeline);
}

void GstreamerPipeline::setFakeSinkHandOffs()
{
    if( m_fakeSink )
    {
       handoff_handler_id = g_signal_connect(m_fakeSink, "handoff", G_CALLBACK(on_handoff), this);
    }
}

void GstreamerPipeline::unSetFakeSinkHandOffs()
{
    if( m_fakeSink && handoff_handler_id != 0)
    {
        g_signal_handler_disconnect(m_fakeSink, handoff_handler_id);
        handoff_handler_id = 0;
    }
}

void GstreamerPipeline::capturePicture(QString filename)
{
    int count = 0;
    m_lastPictureFilename = filename;
    m_IsPictureCaptured = false;

    setFakeSinkHandOffs();

    while (!m_IsPictureCaptured && count <5) {
        usleep(1000000);
        count++;
    }
}

gboolean GstreamerPipeline::bus_call(GstBus *bus, GstMessage *msg, gpointer user_data)
{
    GError *err;
    gchar *debug_info;

    switch (GST_MESSAGE_TYPE(msg)) {
        case GST_MESSAGE_ERROR:
            gst_message_parse_error(msg, &err, &debug_info);
            qDebug() << "Error received: " << err->message;
            qDebug() << "Debug info: " << (debug_info ? debug_info : "None");
            g_clear_error(&err);
            g_free(debug_info);
            break;

        case GST_MESSAGE_WARNING:
            gst_message_parse_warning(msg, &err, &debug_info);
            qDebug() <<"Warning: " << err->message;
            qDebug() <<"Debug info: " << (debug_info ? debug_info : "None");
            g_clear_error(&err);
            g_free(debug_info);
            break;

        case GST_MESSAGE_EOS:
            qDebug() << "!!! End-of-stream reached.";
            break;

        case GST_MESSAGE_STATE_CHANGED:
            if (GST_MESSAGE_SRC(msg) == (GstObject *) user_data ||1) {
                GstState old_state, new_state, pending_state;
                gst_message_parse_state_changed(msg, &old_state, &new_state, &pending_state);
                qDebug() << "State of element " << GST_ELEMENT_NAME(msg->src) << "changed from " <<
                        gst_element_state_get_name(old_state) << "to" <<
                        gst_element_state_get_name(new_state);
            }
            break;

        default:
            break;
    }

    return TRUE;  // Continue watching for messages
}

void GstreamerPipeline::on_handoff(GstElement *fakesink, GstBuffer *buffer, GstPad *pad, gpointer user_data)
{
    GstMapInfo map;
    GstreamerPipeline* currentPipeline = static_cast<GstreamerPipeline*>(user_data);

    if (gst_buffer_map(buffer, &map, GST_MAP_READ))
    {
        // Write buffer data to file
        QImage image(map.data, currentPipeline->m_videoWidth, currentPipeline->m_videoHeight, QImage::Format_RGBX8888);
	image.mirror(true, false);
        if( image.save(currentPipeline->m_lastPictureFilename))
            qDebug() << "Image succesfully saved at location " << currentPipeline->m_lastPictureFilename;
        else
            qDebug() << "Unable to save image at location " << currentPipeline->m_lastPictureFilename;
        gst_buffer_unmap(buffer, &map);
        currentPipeline->unSetFakeSinkHandOffs();
        currentPipeline->m_IsPictureCaptured = true;
    }
    else
    {
        g_printerr("Failed to map buffer.\n");
    }
}
