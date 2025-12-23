#include "settingsmanager.h"
#include "../logging/logger.h"
#include "../ai/IAIProvider.h"
#include <QSettings>
#include <QFileInfo>

SettingsManager* SettingsManager::s_instance = nullptr;

SettingsManager::SettingsManager(QObject *parent)
    : QObject(parent)
    , m_settings(new QSettings("Pix3lForge", "Pix3lForge", this))
{
    LOG_DEBUG("Loading application settings");
}

SettingsManager::~SettingsManager()
{
}

SettingsManager* SettingsManager::instance()
{
    if (!s_instance) {
        s_instance = new SettingsManager();
    }
    return s_instance;
}

QStringList SettingsManager::recentFiles() const
{
    QStringList files = m_settings->value("recentFiles").toStringList();

    // Remove files that no longer exist
    QStringList validFiles;
    for (const QString &file : files) {
        if (QFileInfo::exists(file)) {
            validFiles.append(file);
        }
    }

    // Update settings if any files were removed
    if (validFiles.size() != files.size()) {
        const_cast<SettingsManager*>(this)->m_settings->setValue("recentFiles", validFiles);
    }

    return validFiles;
}

void SettingsManager::addRecentFile(const QString &filePath)
{
    LOG_DEBUG(QString("Adding to recent files: %1").arg(filePath));

    QStringList files = recentFiles();

    // Remove if already exists (to move it to top)
    files.removeAll(filePath);

    // Add to beginning
    files.prepend(filePath);

    // Keep only MaxRecentFiles
    while (files.size() > MaxRecentFiles) {
        files.removeLast();
    }

    m_settings->setValue("recentFiles", files);
    emit recentFilesChanged();
}

void SettingsManager::clearRecentFiles()
{
    m_settings->remove("recentFiles");
    emit recentFilesChanged();
}

QByteArray SettingsManager::windowGeometry() const
{
    return m_settings->value("windowGeometry").toByteArray();
}

void SettingsManager::setWindowGeometry(const QByteArray &geometry)
{
    m_settings->setValue("windowGeometry", geometry);
}

QByteArray SettingsManager::windowState() const
{
    return m_settings->value("windowState").toByteArray();
}

void SettingsManager::setWindowState(const QByteArray &state)
{
    m_settings->setValue("windowState", state);
}

AIProviderConfig SettingsManager::getAIProviderConfig() const
{
    IAIProvider::ProviderType providerType = static_cast<IAIProvider::ProviderType>(
        m_settings->value("AI/providerType", IAIProvider::LMStudio).toInt());

    AIProviderConfig config = AIProviderConfig::getDefaultConfig(providerType);
    config.endpoint = m_settings->value("AI/endpoint", config.endpoint).toString();
    config.apiKey = m_settings->value("AI/apiKey", config.apiKey).toString();
    config.modelName = m_settings->value("AI/modelName", config.modelName).toString();
    config.timeout = m_settings->value("AI/timeout", config.timeout).toInt();
    config.maxRetries = m_settings->value("AI/maxRetries", config.maxRetries).toInt();

    return config;
}

void SettingsManager::setAIProviderConfig(const AIProviderConfig& config)
{
    m_settings->setValue("AI/providerType", static_cast<int>(config.type));
    m_settings->setValue("AI/endpoint", config.endpoint);
    m_settings->setValue("AI/apiKey", config.apiKey);
    m_settings->setValue("AI/modelName", config.modelName);
    m_settings->setValue("AI/timeout", config.timeout);
    m_settings->setValue("AI/maxRetries", config.maxRetries);
}
