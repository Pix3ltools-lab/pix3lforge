#ifndef OPENROUTERPROVIDER_H
#define OPENROUTERPROVIDER_H

#include "../IAIProvider.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <utility>

/**
 * OpenRouter AI Provider implementation
 * Supports 100+ vision models including:
 * - Google Gemini Flash/Pro
 * - Anthropic Claude 3.5 Sonnet/Opus/Haiku
 * - OpenAI GPT-4o/GPT-4o-mini
 * - Meta Llama 3.2 Vision
 * - Qwen2/Qwen3 Vision
 */
class OpenRouterProvider : public IAIProvider
{
    Q_OBJECT

public:
    explicit OpenRouterProvider(QObject* parent = nullptr);
    explicit OpenRouterProvider(const AIProviderConfig& config, QObject* parent = nullptr);
    ~OpenRouterProvider();

    // IAIProvider interface
    void analyzeImageForEnhancements(const QString& imagePath) override;
    bool testConnection() override;
    QString getProviderName() const override { return "OpenRouter"; }
    ProviderType getProviderType() const override { return IAIProvider::OpenRouter; }
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

#endif // OPENROUTERPROVIDER_H
