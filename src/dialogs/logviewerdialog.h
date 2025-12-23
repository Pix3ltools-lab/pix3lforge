#ifndef LOGVIEWERDIALOG_H
#define LOGVIEWERDIALOG_H

#include <QDialog>
#include <QTextEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QPushButton>
#include <QLabel>
#include "../logging/logger.h"

/**
 * LogViewerDialog - Dialog for viewing application logs
 * Responsibility: Display, filter, and manage log files
 * Follows SRP: Only manages log visualization and basic log operations
 */
class LogViewerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LogViewerDialog(QWidget* parent = nullptr);

private slots:
    void onRefreshClicked();
    void onClearLogsClicked();
    void onSaveAsClicked();
    void onLogLevelFilterChanged(int index);
    void onLineCountChanged(int value);
    void onAutoScrollToggled(bool checked);

private:
    void setupUI();
    void loadLogs();
    void applyLogLevelFilter();
    QString colorizeLog(const QString& logText);

    // UI Components
    QTextEdit* m_logTextEdit;
    QComboBox* m_logLevelFilter;
    QSpinBox* m_lineCountSpin;
    QPushButton* m_refreshButton;
    QPushButton* m_clearButton;
    QPushButton* m_saveAsButton;
    QPushButton* m_autoScrollCheckbox;
    QLabel* m_logInfoLabel;

    // Current loaded logs
    QString m_currentLogs;

    // Settings
    Logger::LogLevel m_filterLevel;
    int m_lineCount;
    bool m_autoScroll;
};

#endif // LOGVIEWERDIALOG_H
