#include "AISettingsDialog.h"
#include "../ai/AIProviderFactory.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QMessageBox>

AISettingsDialog::AISettingsDialog(QWidget* parent)
    : QDialog(parent)
    , m_testProvider(nullptr)
{
    setWindowTitle(tr("AI Provider Settings"));
    resize(600, 500);
    setupUI();
}

void AISettingsDialog::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // Provider selection group
    QGroupBox* providerGroup = new QGroupBox(tr("AI Provider"));
    QVBoxLayout* providerLayout = new QVBoxLayout(providerGroup);

    m_providerCombo = new QComboBox();
    m_providerCombo->addItem("LM Studio (Local)", IAIProvider::LMStudio);
    m_providerCombo->addItem("OpenRouter", IAIProvider::OpenRouter);
    m_providerCombo->addItem("OpenAI", IAIProvider::OpenAI);
    m_providerCombo->addItem("Anthropic Claude", IAIProvider::Anthropic);
    connect(m_providerCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &AISettingsDialog::onProviderChanged);
    providerLayout->addWidget(m_providerCombo);

    m_providerDescriptionText = new QTextEdit();
    m_providerDescriptionText->setReadOnly(true);
    m_providerDescriptionText->setMaximumHeight(80);
    providerLayout->addWidget(m_providerDescriptionText);

    mainLayout->addWidget(providerGroup);

    // Configuration group
    QGroupBox* configGroup = new QGroupBox(tr("Configuration"));
    QFormLayout* configLayout = new QFormLayout(configGroup);

    m_endpointEdit = new QLineEdit();
    configLayout->addRow(tr("Endpoint URL:"), m_endpointEdit);

    QHBoxLayout* apiKeyLayout = new QHBoxLayout();
    m_apiKeyEdit = new QLineEdit();
    m_apiKeyEdit->setEchoMode(QLineEdit::Password);
    apiKeyLayout->addWidget(m_apiKeyEdit);

    m_showApiKeyCheckbox = new QCheckBox(tr("Show"));
    connect(m_showApiKeyCheckbox, &QCheckBox::toggled,
            this, &AISettingsDialog::onShowApiKeyToggled);
    apiKeyLayout->addWidget(m_showApiKeyCheckbox);

    configLayout->addRow(tr("API Key:"), apiKeyLayout);

    m_modelCombo = new QComboBox();
    m_modelCombo->setEditable(true);
    configLayout->addRow(tr("Model:"), m_modelCombo);

    m_timeoutSpin = new QSpinBox();
    m_timeoutSpin->setRange(5000, 300000);
    m_timeoutSpin->setSingleStep(5000);
    m_timeoutSpin->setSuffix(" ms");
    m_timeoutSpin->setValue(30000);
    configLayout->addRow(tr("Timeout:"), m_timeoutSpin);

    m_retriesSpin = new QSpinBox();
    m_retriesSpin->setRange(0, 10);
    m_retriesSpin->setValue(3);
    configLayout->addRow(tr("Max Retries:"), m_retriesSpin);

    mainLayout->addWidget(configGroup);

    // Test connection section
    QHBoxLayout* testLayout = new QHBoxLayout();
    m_testConnectionButton = new QPushButton(tr("Test Connection"));
    connect(m_testConnectionButton, &QPushButton::clicked,
            this, &AISettingsDialog::onTestConnectionClicked);
    testLayout->addWidget(m_testConnectionButton);

    m_connectionStatusLabel = new QLabel();
    testLayout->addWidget(m_connectionStatusLabel);
    testLayout->addStretch();

    mainLayout->addLayout(testLayout);

    mainLayout->addStretch();

    // Dialog buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();

    QPushButton* okButton = new QPushButton(tr("OK"));
    connect(okButton, &QPushButton::clicked, this, &QDialog::accept);
    buttonLayout->addWidget(okButton);

    QPushButton* cancelButton = new QPushButton(tr("Cancel"));
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    buttonLayout->addWidget(cancelButton);

    mainLayout->addLayout(buttonLayout);

    // Initialize with first provider
    onProviderChanged(0);
}

void AISettingsDialog::onProviderChanged(int index)
{
    IAIProvider::ProviderType type = static_cast<IAIProvider::ProviderType>(
        m_providerCombo->itemData(index).toInt());

    updateUIForProvider(type);
}

void AISettingsDialog::updateUIForProvider(IAIProvider::ProviderType type)
{
    // Update description
    m_providerDescriptionText->setPlainText(AIProviderFactory::getProviderDescription(type));

    // Update default endpoint
    AIProviderConfig defaultConfig = AIProviderConfig::getDefaultConfig(type);
    m_endpointEdit->setText(defaultConfig.endpoint);

    // Update available models
    m_modelCombo->clear();
    QStringList models = AIProviderFactory::getModelsForProvider(type);
    m_modelCombo->addItems(models);
    m_modelCombo->setCurrentText(defaultConfig.modelName);

    // Update timeout
    m_timeoutSpin->setValue(defaultConfig.timeout);

    // Update retries
    m_retriesSpin->setValue(defaultConfig.maxRetries);

    // Enable/disable API key field
    bool needsApiKey = AIProviderFactory::requiresApiKey(type);
    m_apiKeyEdit->setEnabled(needsApiKey);
    m_showApiKeyCheckbox->setEnabled(needsApiKey);

    if (!needsApiKey) {
        m_apiKeyEdit->clear();
        m_apiKeyEdit->setPlaceholderText(tr("No API key required"));
    } else {
        m_apiKeyEdit->setPlaceholderText(tr("Enter your API key"));
    }

    // Clear connection status
    m_connectionStatusLabel->clear();
}

void AISettingsDialog::onTestConnectionClicked()
{
    m_connectionStatusLabel->setText(tr("Testing connection..."));
    m_connectionStatusLabel->setStyleSheet("color: blue;");
    m_testConnectionButton->setEnabled(false);

    // Create temporary provider for testing
    AIProviderConfig config = getConfig();

    if (m_testProvider) {
        delete m_testProvider;
    }

    m_testProvider = AIProviderFactory::createProvider(config, this);

    if (!m_testProvider) {
        m_connectionStatusLabel->setText(tr("Failed to create provider"));
        m_connectionStatusLabel->setStyleSheet("color: red;");
        m_testConnectionButton->setEnabled(true);
        return;
    }

    connect(m_testProvider, &IAIProvider::connectionTestResult,
            this, &AISettingsDialog::onConnectionTestResult);

    m_testProvider->testConnection();
}

void AISettingsDialog::onConnectionTestResult(bool success, const QString& message)
{
    if (success) {
        m_connectionStatusLabel->setText(tr("✓ %1").arg(message));
        m_connectionStatusLabel->setStyleSheet("color: green;");
    } else {
        m_connectionStatusLabel->setText(tr("✗ %1").arg(message));
        m_connectionStatusLabel->setStyleSheet("color: red;");
    }

    m_testConnectionButton->setEnabled(true);

    if (m_testProvider) {
        m_testProvider->deleteLater();
        m_testProvider = nullptr;
    }
}

void AISettingsDialog::onShowApiKeyToggled(bool checked)
{
    m_apiKeyEdit->setEchoMode(checked ? QLineEdit::Normal : QLineEdit::Password);
}

AIProviderConfig AISettingsDialog::getConfig() const
{
    AIProviderConfig config;
    config.type = static_cast<IAIProvider::ProviderType>(
        m_providerCombo->currentData().toInt());
    config.endpoint = m_endpointEdit->text();
    config.apiKey = m_apiKeyEdit->text();
    config.modelName = m_modelCombo->currentText();
    config.timeout = m_timeoutSpin->value();
    config.maxRetries = m_retriesSpin->value();

    return config;
}

void AISettingsDialog::setConfig(const AIProviderConfig& config)
{
    // Find and set provider
    for (int i = 0; i < m_providerCombo->count(); ++i) {
        if (m_providerCombo->itemData(i).toInt() == config.type) {
            m_providerCombo->setCurrentIndex(i);
            break;
        }
    }

    m_endpointEdit->setText(config.endpoint);
    m_apiKeyEdit->setText(config.apiKey);
    m_modelCombo->setCurrentText(config.modelName);
    m_timeoutSpin->setValue(config.timeout);
    m_retriesSpin->setValue(config.maxRetries);
}
