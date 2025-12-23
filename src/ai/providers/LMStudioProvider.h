#ifndef LMSTUDIOPROVIDER_H
#define LMSTUDIOPROVIDER_H

#include "../IAIProvider.h"
#include <QNetworkAccessManager>

/**
 * LM Studio AI Provider (Local)
 * Communicates with local LM Studio server
 * No API key required
 */
class LMStudioProvider : public IAIProvider
{
    Q_OBJECT

public:
    explicit LMStudioProvider(QObject* parent = nullptr);
    explicit LMStudioProvider(const AIProviderConfig& config, QObject* parent = nullptr);
    ~LMStudioProvider();

    // IAIProvider interface
    void analyzeImageForEnhancements(const QString& imagePath) override;
    bool testConnection() override;
    QString getProviderName() const override { return "LM Studio"; }
    ProviderType getProviderType() const override { return IAIProvider::LMStudio; }
    QString getEndpoint() const override { return m_serverUrl; }
    void setEndpoint(const QString& endpoint) override;
    QString getModelName() const override { return m_modelName; }
    void setModelName(const QString& modelName) override { m_modelName = modelName; }
    QStringList getAvailableModels() override;

    void setTimeout(int timeoutMs) { m_timeout = timeoutMs; }
    void setMaxRetries(int retries) { m_maxRetries = retries; }

private:
    std::pair<bool, QByteArray> performRequestSync(const QByteArray& jsonData);

    QNetworkAccessManager* m_networkManager;
    QString m_serverUrl;
    QString m_modelName;
    int m_timeout;
    int m_maxRetries;
};

#endif // LMSTUDIOPROVIDER_H
