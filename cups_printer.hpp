#ifndef CUPS_PRINTER_HPP
#define CUPS_PRINTER_HPP

#include <string>
#include "printer.hpp"

// Forward declaration of typdef (please do not typedef public structure)
struct cups_dest_s;
typedef struct cups_dest_s cups_dest_t;
struct cups_option_s;
typedef struct cups_option_s cups_option_t;
struct _cups_dinfo_s;
typedef struct _cups_dinfo_s cups_dinfo_t;
class QTimer;

class CupsPrinter: public Printer
{
    Q_OBJECT
public:
    CupsPrinter(QObject *parent = nullptr);
    ~CupsPrinter();

    virtual bool isPrinterReady() override;
    virtual QString getPrinterErrorMessage() override;
    virtual void printFile(const QString &filePath) override;
    virtual void clearPrinterError() override;

private:
    int m_numberOfPrinter;
    cups_dest_t *m_printers;
    cups_dest_t *m_selectedPrinter;
    std::string m_lastError;
    int m_numOptions;
    cups_option_t *m_options;
    cups_dinfo_t *m_info;
    QTimer *m_jobMonitorTimer;
    int m_currentJobId;

    int lookForLocalPrinters();
    void initCupsOptions();
    int createJob();
    bool writeJob(int job_id, std::string filePath);
    void checkJobStatus();
    void monitorJob(int job_id);
    void readPrinterError();
    void resumePrinter();
    void cancelJob2Printer();
};

#endif // CUPS_PRINTER_HPP