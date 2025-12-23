#ifndef OPENAIPROVIDER_H
#define OPENAIPROVIDER_H

#include "../IAIProvider.h"
#include <QNetworkAccessManager>

/**
 * OpenAI AI Provider
 * Supports GPT-4o, GPT-4o-mini with vision
 * Requires API key from https://platform.openai.com/api-keys
 */
class OpenAIProvider : public IAIProvider
{
    Q_OBJECT

public:
    explicit OpenAIProvider(QObject* parent = nullptr);
    explicit OpenAIProvider(const AIProviderConfig& config, QObject* parent = nullptr);
    ~OpenAIProvider();

    // IAIProvider interface
    void analyzeImageForEnhancements(const QString& imagePath) override;
    bool testConnection() override;
    QString getProviderName() const override { return "OpenAI"; }
    ProviderType getProviderType() const override { return IAIProvider::OpenAI; }
    QString getEndpoint() const override { return m_endpoint; }
    void setEndpoint(const QString& endpoint) override { m_endpoint = endpoint; }
    QString getModelName() const override { return m_modelName; }
    void setModelName(const QString& modelName) override { m_modelName = modelName; }
    QStringList getAvailableModels() override;

    void setApiKey(const QString& apiKey) { m_apiKey = apiKey; }
    void setTimeout(int timeoutMs) { m_timeout = timeoutMs; }
    void setMaxRetries(int retries) { m_maxRetries = retries; }

    static QStringList getRecommendedModels();

private:
    std::pair<bool, QByteArray> performRequestSync(const QByteArray& jsonData);

    QNetworkAccessManager* m_networkManager;
    QString m_endpoint;
    QString m_apiKey;
    QString m_modelName;
    int m_timeout;
    int m_maxRetries;
};

#endif // OPENAIPROVIDER_H
