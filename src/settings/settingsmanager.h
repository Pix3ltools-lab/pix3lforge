#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <QObject>
#include <QStringList>

class QSettings;
struct AIProviderConfig;

/**
 * @class SettingsManager
 * @brief Singleton class for managing application settings
 *
 * This class centralizes all QSettings access across the application,
 * providing a single point of truth for configuration management.
 * It handles recent files, window geometry, and other persistent settings.
 */
class SettingsManager : public QObject
{
    Q_OBJECT

public:
    static SettingsManager* instance();

    // Recent files management
    QStringList recentFiles() const;
    void addRecentFile(const QString &filePath);
    void clearRecentFiles();
    static constexpr int MaxRecentFiles = 5;

    // Window geometry management
    QByteArray windowGeometry() const;
    void setWindowGeometry(const QByteArray &geometry);
    QByteArray windowState() const;
    void setWindowState(const QByteArray &state);

    // AI Configuration methods
    AIProviderConfig getAIProviderConfig() const;
    void setAIProviderConfig(const AIProviderConfig& config);

signals:
    void recentFilesChanged();

private:
    explicit SettingsManager(QObject *parent = nullptr);
    ~SettingsManager();

    // Prevent copying
    SettingsManager(const SettingsManager&) = delete;
    SettingsManager& operator=(const SettingsManager&) = delete;

    static SettingsManager *s_instance;
    QSettings *m_settings;
};

#endif // SETTINGSMANAGER_H
