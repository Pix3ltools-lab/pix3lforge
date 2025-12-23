#ifndef ANTHROPICPROVIDER_H
#define ANTHROPICPROVIDER_H

#include "../IAIProvider.h"
#include <QNetworkAccessManager>

/**
 * Anthropic AI Provider
 * Supports Claude 3.5 Sonnet, Claude 3 Opus/Sonnet/Haiku with vision
 * Requires API key from https://console.anthropic.com/
 *
 * Note: Anthropic API uses a different message format (not OpenAI-compatible)
 */
class AnthropicProvider : public IAIProvider
{
    Q_OBJECT

public:
    explicit AnthropicProvider(QObject* parent = nullptr);
    explicit AnthropicProvider(const AIProviderConfig& config, QObject* parent = nullptr);
    ~AnthropicProvider();

    // IAIProvider interface
    void analyzeImageForEnhancements(const QString& imagePath) override;
    bool testConnection() override;
    QString getProviderName() const override { return "Anthropic Claude"; }
    ProviderType getProviderType() const override { return IAIProvider::Anthropic; }
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

#endif // ANTHROPICPROVIDER_H
