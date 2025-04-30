#include "cups_printer.hpp"

#include <iostream>
#include <cups/cups.h>
#include <QTimer>

typedef struct
{
    int num_dests;
    cups_dest_t *dests;
} printersData;

static int forEachPrinterCallback(printersData *user_data, unsigned flags, cups_dest_t *dest)
{
    std::cout << "Processing a printer from callback!!! (flags = " << flags << ")" << std::endl;
    if (flags & CUPS_DEST_FLAGS_REMOVED)
    {
        user_data->num_dests =
            cupsRemoveDest(dest->name, dest->instance,
                           user_data->num_dests,
                           &(user_data->dests));
    }
    else
    {
        user_data->num_dests =
            cupsCopyDest(dest, user_data->num_dests,
                         &(user_data->dests));
    }

    return (1);
}

CupsPrinter::CupsPrinter(QObject *parent)
    : m_jobMonitorTimer(nullptr)
{
    std::cout << "Creator of CupsPrinter" << std::endl;
    this->lookForLocalPrinters();

    for(int i = 0; i < m_numberOfPrinter; i++)
    {
        if( m_printers[i].is_default )
        {
            m_selectedPrinter = &m_printers[i];
            std::cout << "Current printer set to " << m_selectedPrinter->name << std::endl;
            break;
        }
    }

    this->initCupsOptions();
    m_info = cupsCopyDestInfo(CUPS_HTTP_DEFAULT, m_selectedPrinter);
}

CupsPrinter::~CupsPrinter()
{
    if(m_info)
        cupsFreeDestInfo(m_info);
    if (m_options)
        cupsFreeOptions(m_numOptions, m_options);
}

int CupsPrinter::lookForLocalPrinters()
{
    printersData data = { 0, NULL };

    std::cout << "Look for printers" << std::endl;
    if(!cupsEnumDests(  CUPS_DEST_FLAGS_NONE,
                        5000,
                        NULL,
                        CUPS_PRINTER_LOCAL,
                        CUPS_PRINTER_DISCOVERED,
                        (cups_dest_cb_t)forEachPrinterCallback,
                        &data))
    {
        std::cout << "Oupsi, I got a issue" << std::endl;
        cupsFreeDests(data.num_dests, data.dests);
        m_printers = NULL;
        return (0);
    }
    m_printers = data.dests;
    m_numberOfPrinter = data.num_dests;
    return data.num_dests;
}

bool CupsPrinter::isPrinterReady()
{
    const char* state = cupsGetOption(  "printer-state",
                                m_selectedPrinter->num_options,
                                m_selectedPrinter->options);
    std::cout << "Is Printer Ready? " << (state && strncmp(state, "3", 1) == 0) << "(state=" << state << ")" << std::endl;
    return (state && strncmp(state, "3", 1) == 0);
}

void CupsPrinter::initCupsOptions()
{
    m_numOptions = 0;
    m_options = NULL;
    m_numOptions = cupsAddOption(CUPS_COPIES, "1", m_numOptions, &m_options);
    m_numOptions = cupsAddOption(CUPS_SIDES, CUPS_SIDES_ONE_SIDED, m_numOptions, &m_options);
    m_numOptions = cupsAddOption(CUPS_PRINT_QUALITY, CUPS_PRINT_QUALITY_HIGH, m_numOptions, &m_options);
    m_numOptions = cupsAddOption(CUPS_MEDIA_TYPE, CUPS_MEDIA_TYPE_PHOTO_GLOSSY, m_numOptions, &m_options);
    m_numOptions = cupsAddOption("StpBorderless", "true", m_numOptions, &m_options);
    // m_numOptions = cupsAddOption("fill", "true", m_numOptions, &m_options);
}

QString CupsPrinter::getPrinterErrorMessage()
{
    std::cout << "returning error as QString " << QString(m_lastError.c_str()).toStdString() << std::endl;
    return QString(m_lastError.c_str());
}

int CupsPrinter::createJob()
{
    int job_id = -1;

    ipp_status_t status = cupsCreateDestJob(CUPS_HTTP_DEFAULT, m_selectedPrinter, m_info,
                        &job_id, "My Picture", m_numOptions, m_options);
    if (status == IPP_STATUS_OK)
        std::cout << "Created job: " << job_id << std::endl;
    else
    {
        m_lastError = std::string(cupsLastErrorString());
        std::cout << "Unable to create job (status=" << status << "): " << m_lastError << std::endl;
        emit printerRaisedError();
    }
    
    return job_id;
}

bool CupsPrinter::writeJob(int job_id, std::string filePath)
{
    FILE *fp = fopen(filePath.c_str(), "rb");
    size_t bytes;
    bool returnStatus = true;
    char buffer[65536];

    if(fp == NULL)
        return false;

    if (cupsStartDestDocument(CUPS_HTTP_DEFAULT, m_selectedPrinter, m_info,
                            job_id, filePath.c_str(), CUPS_FORMAT_JPEG, m_numOptions, m_options, 1) == HTTP_STATUS_CONTINUE)
    {
        while ((bytes = fread(buffer, 1, sizeof(buffer), fp)) > 0)
        {
            if (cupsWriteRequestData(CUPS_HTTP_DEFAULT, buffer, bytes) != HTTP_STATUS_CONTINUE)
                break;
        }

        if (cupsFinishDestDocument(CUPS_HTTP_DEFAULT, m_selectedPrinter, m_info) == IPP_STATUS_OK)
            std::cout << "Picture printed with success!!!" << std::endl;
        else
        {
            m_lastError = std::string(cupsLastErrorString());
            std::cout << "Unable to print picture: " << m_lastError << std::endl;
            emit printerRaisedError();
            returnStatus = false;
        }
    }
    else
    {
        m_lastError = std::string(cupsLastErrorString());
        std::cout << "Unable to print picture: " << m_lastError << std::endl;
        emit printerRaisedError();
        returnStatus = false;
    }

    fclose(fp);
    return returnStatus;
}

void CupsPrinter::printFile(const QString &filePath)
{
    std::cout << "Trying to print with cups printer" << filePath.toStdString() << std::endl;

    if( isPrinterReady() )
    {
        int job_id = this->createJob();

        if( job_id > 0 )
        {
            if(this->writeJob(job_id, filePath.toStdString()))
                this->monitorJob(job_id);
        }
    }
    else
    {
        std::cout << "Printer not ready, cannot print" << filePath.toStdString() << std::endl;
    }
}

void CupsPrinter::monitorJob(int job_id)
{
    std::cout << "Trying to monitor the job " << job_id << std::endl;
    if (m_jobMonitorTimer)
    {
        m_jobMonitorTimer->stop();
        delete m_jobMonitorTimer;
    }

    m_currentJobId = job_id;
    m_jobMonitorTimer = new QTimer(this);
    connect(m_jobMonitorTimer, &QTimer::timeout, this, &CupsPrinter::checkJobStatus);
    m_jobMonitorTimer->start(1000); // Check every 1 second
}

void CupsPrinter::checkJobStatus()
{
    cups_job_t *jobs = nullptr;
    int num_jobs = cupsGetJobs(&jobs, NULL, 0, CUPS_WHICHJOBS_ACTIVE);
    std::cout << "Number of job to look for mine " << num_jobs << std::endl;

    for (int i = 0; i < num_jobs && jobs; ++i)
    {
        if (jobs[i].id == m_currentJobId)
        {
            std::cout << "Job " << m_currentJobId << " state: " << jobs[i].state << std::endl;

            switch (jobs[i].state)
            {
                case IPP_JSTATE_PROCESSING:
                case IPP_JSTATE_HELD:
                    // Still working
                    break;

                case IPP_JSTATE_PENDING:
                    // I need to understand why the job is pending
                    m_lastError.clear();
                    this->readPrinterError();
                    if( !m_lastError.empty())
                    {
                        std::cout << "Last Error " << m_lastError << std::endl;
                        emit printerRaisedError();
                        m_jobMonitorTimer->stop();
                    }
                    break;

                case IPP_JSTATE_COMPLETED:
                    std::cout << "Job completed successfully" << std::endl;
                    m_jobMonitorTimer->stop();
                    break;

                case IPP_JSTATE_ABORTED:
                case IPP_JSTATE_CANCELED:
                case IPP_JSTATE_STOPPED:
                    std::cout << "Job failed or was stopped" << std::endl;
                    this->readPrinterError();
                    emit printerRaisedError();
                    m_jobMonitorTimer->stop();
                    break;

                default:
                    break;
            }
            break;
        }
    }

    if(jobs)
        cupsFreeJobs(num_jobs, jobs);
}

void CupsPrinter::readPrinterError()
{
    if(m_info)
    {
        cupsFreeDestInfo(m_info);
    }
    m_info = cupsCopyDestInfo(CUPS_HTTP_DEFAULT, m_selectedPrinter);
    const char *reasons = cupsGetOption("printer-state-reasons", m_selectedPrinter->num_options, m_selectedPrinter->options);

    if (reasons)
    {
        if (strstr(reasons, "media-empty") || strstr(reasons, "media-needed"))
           m_lastError = "Cassette de papiers vide, vous devez remettre du papier avant d'imprimer une photo.";
        else if (strstr(reasons, "marker-supply-low") || strstr(reasons, "marker-supply-empty"))
            m_lastError = "Cartouche d'encre vide, vous devez changer la cartouche avant de pouvoir imprimer une photo.";
        else if (strstr(reasons, "paused"))
            m_lastError = "Printer is paused!";
        else
            m_lastError = reasons;
    }
}

void CupsPrinter::clearPrinterError()
{
    this->cancelJob2Printer();

    this->resumePrinter(); 
}

void CupsPrinter::resumePrinter()
{
    int cancel = 0;
    http_t *http = httpConnect2(cupsServer(), ippPort(), NULL, AF_UNSPEC, HTTP_ENCRYPT_IF_REQUESTED, 1, 3000, &cancel);
    if (!http) {
        std::cerr << "Unable to connect to CUPS server." << std::endl;
        return;
    }

    // Build the printer URI
    char uri[1024];
    snprintf(uri, sizeof(uri), "ipp://localhost/printers/%s", m_selectedPrinter->name);

    ipp_t *request = ippNewRequest(IPP_RESUME_PRINTER);
    ippAddString(request, IPP_TAG_OPERATION, IPP_TAG_URI, "printer-uri", NULL, uri);

    ipp_t *response = cupsDoRequest(http, request, "/admin/");

    if (cupsLastError() > IPP_STATUS_OK_EVENTS_COMPLETE) {
        std::cerr << "Failed to resume printer: " << cupsLastErrorString() << std::endl;
        ippDelete(response);
        httpClose(http);
        return;
    }

    ippDelete(response);
    httpClose(http);
}

void CupsPrinter::cancelJob2Printer()
{
    if (m_currentJobId <= 0)
        return;

    http_t *http = httpConnect2(cupsServer(), ippPort(), NULL, AF_UNSPEC, HTTP_ENCRYPT_IF_REQUESTED, 1, 3000, NULL);
    if (!http) {
        std::cerr << "Unable to connect to CUPS server." << std::endl;
        return;
    }

    char uri[1024];
    snprintf(uri, sizeof(uri), "ipp://localhost/printers/%s", m_selectedPrinter->name);

    ipp_t *request = ippNewRequest(IPP_CANCEL_JOB);
    ippAddString(request, IPP_TAG_OPERATION, IPP_TAG_URI, "printer-uri", NULL, uri);
    ippAddInteger(request, IPP_TAG_OPERATION, IPP_TAG_INTEGER, "job-id", m_currentJobId);

    ipp_t *response = cupsDoRequest(http, request, "/jobs/");

    if (cupsLastError() > IPP_STATUS_OK_EVENTS_COMPLETE) {
        std::cerr << "Failed to cancel job: " << cupsLastErrorString() << std::endl;
    }

    ippDelete(response);
    httpClose(http);

    m_currentJobId = -1;
}


