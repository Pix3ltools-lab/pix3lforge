#ifndef AISETTINGSDIALOG_H
#define AISETTINGSDIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QCheckBox>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include "../ai/IAIProvider.h"

/**
 * Dialog for configuring AI provider settings
 */
class AISettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AISettingsDialog(QWidget* parent = nullptr);

    AIProviderConfig getConfig() const;
    void setConfig(const AIProviderConfig& config);

private slots:
    void onProviderChanged(int index);
    void onTestConnectionClicked();
    void onConnectionTestResult(bool success, const QString& message);
    void onShowApiKeyToggled(bool checked);

private:
    void setupUI();
    void updateUIForProvider(IAIProvider::ProviderType type);

    QComboBox* m_providerCombo;
    QTextEdit* m_providerDescriptionText;
    QLineEdit* m_endpointEdit;
    QLineEdit* m_apiKeyEdit;
    QCheckBox* m_showApiKeyCheckbox;
    QComboBox* m_modelCombo;
    QSpinBox* m_timeoutSpin;
    QSpinBox* m_retriesSpin;
    QPushButton* m_testConnectionButton;
    QLabel* m_connectionStatusLabel;

    IAIProvider* m_testProvider;
};

#endif // AISETTINGSDIALOG_H
