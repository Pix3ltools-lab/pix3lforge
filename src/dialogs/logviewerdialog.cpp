#include "logviewerdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QFormLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QFile>
#include <QCheckBox>
#include <QScrollBar>

LogViewerDialog::LogViewerDialog(QWidget* parent)
    : QDialog(parent)
    , m_filterLevel(Logger::Debug)
    , m_lineCount(1000)
    , m_autoScroll(true)
{
    setWindowTitle(tr("Log Viewer"));
    resize(900, 600);

    setupUI();
    loadLogs();
}

void LogViewerDialog::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // Control panel
    QGroupBox* controlGroup = new QGroupBox(tr("Controls"));
    QHBoxLayout* controlLayout = new QHBoxLayout(controlGroup);

    // Log level filter
    controlLayout->addWidget(new QLabel(tr("Filter Level:")));
    m_logLevelFilter = new QComboBox();
    m_logLevelFilter->addItem(tr("All (Debug+)"), Logger::Debug);
    m_logLevelFilter->addItem(tr("Info+"), Logger::Info);
    m_logLevelFilter->addItem(tr("Warning+"), Logger::Warning);
    m_logLevelFilter->addItem(tr("Error+"), Logger::Error);
    m_logLevelFilter->addItem(tr("Critical Only"), Logger::Critical);
    m_logLevelFilter->setCurrentIndex(0);  // Default: All
    connect(m_logLevelFilter, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &LogViewerDialog::onLogLevelFilterChanged);
    controlLayout->addWidget(m_logLevelFilter);

    controlLayout->addSpacing(20);

    // Line count
    controlLayout->addWidget(new QLabel(tr("Lines:")));
    m_lineCountSpin = new QSpinBox();
    m_lineCountSpin->setRange(100, 10000);
    m_lineCountSpin->setValue(1000);
    m_lineCountSpin->setSingleStep(100);
    m_lineCountSpin->setToolTip(tr("Number of log lines to display"));
    connect(m_lineCountSpin, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &LogViewerDialog::onLineCountChanged);
    controlLayout->addWidget(m_lineCountSpin);

    controlLayout->addSpacing(20);

    // Auto-scroll checkbox
    QCheckBox* autoScrollCheck = new QCheckBox(tr("Auto-scroll"));
    autoScrollCheck->setChecked(true);
    autoScrollCheck->setToolTip(tr("Automatically scroll to the end when loading logs"));
    connect(autoScrollCheck, &QCheckBox::toggled, this, &LogViewerDialog::onAutoScrollToggled);
    controlLayout->addWidget(autoScrollCheck);

    controlLayout->addStretch();

    // Action buttons
    m_refreshButton = new QPushButton(tr("Refresh"));
    m_refreshButton->setToolTip(tr("Reload logs from file"));
    connect(m_refreshButton, &QPushButton::clicked, this, &LogViewerDialog::onRefreshClicked);
    controlLayout->addWidget(m_refreshButton);

    m_saveAsButton = new QPushButton(tr("Save As..."));
    m_saveAsButton->setToolTip(tr("Save current logs to a file"));
    connect(m_saveAsButton, &QPushButton::clicked, this, &LogViewerDialog::onSaveAsClicked);
    controlLayout->addWidget(m_saveAsButton);

    m_clearButton = new QPushButton(tr("Clear Logs"));
    m_clearButton->setToolTip(tr("Delete all log files (cannot be undone)"));
    connect(m_clearButton, &QPushButton::clicked, this, &LogViewerDialog::onClearLogsClicked);
    controlLayout->addWidget(m_clearButton);

    mainLayout->addWidget(controlGroup);

    // Log text area
    m_logTextEdit = new QTextEdit();
    m_logTextEdit->setReadOnly(true);
    m_logTextEdit->setLineWrapMode(QTextEdit::NoWrap);
    m_logTextEdit->setFont(QFont("Courier New", 9));
    mainLayout->addWidget(m_logTextEdit);

    // Info label
    m_logInfoLabel = new QLabel();
    m_logInfoLabel->setStyleSheet("QLabel { color: #666; font-style: italic; padding: 5px; }");
    mainLayout->addWidget(m_logInfoLabel);

    // Close button
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    QPushButton* closeButton = new QPushButton(tr("Close"));
    connect(closeButton, &QPushButton::clicked, this, &QDialog::accept);
    buttonLayout->addWidget(closeButton);
    mainLayout->addLayout(buttonLayout);
}

void LogViewerDialog::loadLogs()
{
    // Read logs from Logger
    m_currentLogs = Logger::instance().readLastLines(m_lineCount);

    // Apply filter
    applyLogLevelFilter();

    // Update info label
    QString logPath = Logger::instance().getLogFilePath();
    qint64 logSize = Logger::instance().getLogFileSize();
    double logSizeMB = logSize / (1024.0 * 1024.0);

    m_logInfoLabel->setText(tr("Log file: %1 | Size: %2 MB | Showing last %3 lines")
                            .arg(logPath)
                            .arg(logSizeMB, 0, 'f', 2)
                            .arg(m_lineCount));

    // Auto-scroll to bottom if enabled
    if (m_autoScroll) {
        QScrollBar* scrollBar = m_logTextEdit->verticalScrollBar();
        scrollBar->setValue(scrollBar->maximum());
    }
}

void LogViewerDialog::applyLogLevelFilter()
{
    if (m_currentLogs.isEmpty()) {
        m_logTextEdit->setPlainText(tr("No logs available."));
        return;
    }

    // Filter logs by level
    QStringList lines = m_currentLogs.split('\n');
    QStringList filteredLines;

    for (const QString& line : lines) {
        if (line.trimmed().isEmpty()) {
            continue;
        }

        // Check log level in line (format: [timestamp] [LEVEL] [component] message)
        bool shouldInclude = false;

        if (m_filterLevel == Logger::Debug) {
            shouldInclude = true;  // Show all
        } else {
            // Check if line contains appropriate level
            if (m_filterLevel == Logger::Info) {
                shouldInclude = line.contains("[INFO]") || line.contains("[WARNING]") ||
                               line.contains("[ERROR]") || line.contains("[CRITICAL]");
            } else if (m_filterLevel == Logger::Warning) {
                shouldInclude = line.contains("[WARNING]") || line.contains("[ERROR]") ||
                               line.contains("[CRITICAL]");
            } else if (m_filterLevel == Logger::Error) {
                shouldInclude = line.contains("[ERROR]") || line.contains("[CRITICAL]");
            } else if (m_filterLevel == Logger::Critical) {
                shouldInclude = line.contains("[CRITICAL]");
            }
        }

        if (shouldInclude) {
            filteredLines.append(line);
        }
    }

    // Colorize and display
    QString filteredText = filteredLines.join('\n');
    QString colorizedText = colorizeLog(filteredText);
    m_logTextEdit->setHtml(colorizedText);
}

QString LogViewerDialog::colorizeLog(const QString& logText)
{
    // Convert plain text to HTML with colors for different log levels
    QString html = "<pre style='font-family: Courier New, monospace; font-size: 9pt;'>";

    QStringList lines = logText.split('\n');
    for (const QString& line : lines) {
        if (line.isEmpty()) {
            continue;
        }

        QString coloredLine = line.toHtmlEscaped();

        // Color by log level
        if (line.contains("[DEBUG]")) {
            coloredLine = QString("<span style='color: #888;'>%1</span>").arg(coloredLine);
        } else if (line.contains("[INFO]")) {
            coloredLine = QString("<span style='color: #2196F3;'>%1</span>").arg(coloredLine);
        } else if (line.contains("[WARNING]")) {
            coloredLine = QString("<span style='color: #FF9800;'>%1</span>").arg(coloredLine);
        } else if (line.contains("[ERROR]")) {
            coloredLine = QString("<span style='color: #F44336;'>%1</span>").arg(coloredLine);
        } else if (line.contains("[CRITICAL]")) {
            coloredLine = QString("<span style='color: #D32F2F; font-weight: bold;'>%1</span>").arg(coloredLine);
        }

        html += coloredLine + "\n";
    }

    html += "</pre>";
    return html;
}

void LogViewerDialog::onRefreshClicked()
{
    loadLogs();
}

void LogViewerDialog::onClearLogsClicked()
{
    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        tr("Clear Logs"),
        tr("Are you sure you want to delete all log files?\n\n"
        "This action cannot be undone."),
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No
    );

    if (reply == QMessageBox::Yes) {
        int deletedCount = Logger::instance().clearLogs();
        QMessageBox::information(this, tr("Logs Cleared"),
                                tr("Successfully deleted %1 log file(s).").arg(deletedCount));
        loadLogs();
    }
}

void LogViewerDialog::onSaveAsClicked()
{
    QString fileName = QFileDialog::getSaveFileName(
        this,
        tr("Save Logs As"),
        "pix3lforge_log.txt",
        tr("Text Files (*.txt);;Log Files (*.log);;All Files (*.*)")
    );

    if (fileName.isEmpty()) {
        return;
    }

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, tr("Error"), tr("Failed to save log file:\n") + file.errorString());
        return;
    }

    QTextStream out(&file);
    out << m_currentLogs;
    file.close();

    QMessageBox::information(this, tr("Success"), tr("Logs saved successfully to:\n") + fileName);
}

void LogViewerDialog::onLogLevelFilterChanged(int index)
{
    m_filterLevel = static_cast<Logger::LogLevel>(m_logLevelFilter->itemData(index).toInt());
    applyLogLevelFilter();
}

void LogViewerDialog::onLineCountChanged(int value)
{
    m_lineCount = value;
    loadLogs();
}

void LogViewerDialog::onAutoScrollToggled(bool checked)
{
    m_autoScroll = checked;
}
