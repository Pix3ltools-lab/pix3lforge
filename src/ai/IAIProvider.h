#ifndef IAIPROVIDER_H
#define IAIPROVIDER_H

#include <QObject>
#include <QString>
#include "EnhancementResponseParser.h"

/**
 * Abstract interface for AI vision providers
 * Allows pluggable AI backends (LM Studio, OpenRouter, OpenAI, Anthropic)
 *
 * Design Pattern: Strategy pattern for AI provider selection
 */
class IAIProvider : public QObject
{
    Q_OBJECT

public:
    /**
     * Supported AI provider types
     */
    enum ProviderType {
        LMStudio,      // Local LM Studio server
        OpenRouter,    // OpenRouter API (multi-model gateway)
        OpenAI,        // OpenAI GPT-4 Vision API
        Anthropic      // Anthropic Claude 3 Vision API
    };
    Q_ENUM(ProviderType)

    explicit IAIProvider(QObject* parent = nullptr) : QObject(parent) {}
    virtual ~IAIProvider() = default;

    /**
     * Analyze an image and suggest enhancements
     * @param imagePath Path to the image file
     */
    virtual void analyzeImageForEnhancements(const QString& imagePath) = 0;

    /**
     * Test connection to the AI provider
     * @return true if connection successful, false otherwise
     */
    virtual bool testConnection() = 0;

    /**
     * Get human-readable provider name
     */
    virtual QString getProviderName() const = 0;

    /**
     * Get provider type enum
     */
    virtual ProviderType getProviderType() const = 0;

    /**
     * Get/Set endpoint URL
     */
    virtual QString getEndpoint() const = 0;
    virtual void setEndpoint(const QString& endpoint) = 0;

    /**
     * Get/Set model name
     */
    virtual QString getModelName() const = 0;
    virtual void setModelName(const QString& modelName) = 0;

    /**
     * Get available models from provider
     */
    virtual QStringList getAvailableModels() = 0;

signals:
    /**
     * Emitted when enhancement analysis completes successfully
     * @param analysis Structure with all suggested enhancements
     */
    void enhancementAnalysisCompleted(const ImageEnhancementAnalysis& analysis);

    /**
     * Emitted when an error occurs
     * @param error Error message
     */
    void analysisError(const QString& error);

    /**
     * Emitted when connection test completes
     * @param success True if connection successful
     * @param message Status message (error details if failed)
     */
    void connectionTestResult(bool success, const QString& message);
};

/**
 * Configuration structure for AI provider
 */
struct AIProviderConfig
{
    IAIProvider::ProviderType type;
    QString endpoint;          // e.g., "http://localhost:1234" or "https://openrouter.ai/api/v1"
    QString apiKey;            // Empty for LM Studio, required for cloud providers
    QString modelName;         // e.g., "llava" or "google/gemini-flash-1.5-8b"
    int timeout;               // Request timeout in milliseconds (default: 30000)
    int maxRetries;            // Maximum retry attempts (default: 3)

    // Default constructor
    AIProviderConfig()
        : type(IAIProvider::LMStudio)
        , endpoint("http://localhost:1234")
        , apiKey("")
        , modelName("llava")
        , timeout(30000)
        , maxRetries(3)
    {}

    // Helper: Get default config for a provider type
    static AIProviderConfig getDefaultConfig(IAIProvider::ProviderType type)
    {
        switch (type) {
        case IAIProvider::LMStudio:
            return AIProviderConfig(
                IAIProvider::LMStudio,
                "http://localhost:1234",
                "",  // No API key needed
                "google/gemma-3-27b",
                120000,  // 120 seconds for local vision models
                3
            );
        case IAIProvider::OpenRouter:
            return AIProviderConfig(
                IAIProvider::OpenRouter,
                "https://openrouter.ai/api/v1/chat/completions",
                "",  // User must provide API key
                "google/gemini-flash-1.5-8b",
                30000,
                3
            );
        case IAIProvider::OpenAI:
            return AIProviderConfig(
                IAIProvider::OpenAI,
                "https://api.openai.com/v1/chat/completions",
                "",  // User must provide API key
                "gpt-4o",
                30000,
                3
            );
        case IAIProvider::Anthropic:
            return AIProviderConfig(
                IAIProvider::Anthropic,
                "https://api.anthropic.com/v1/messages",
                "",  // User must provide API key
                "claude-3-5-sonnet-20241022",
                30000,
                3
            );
        default:
            return AIProviderConfig();
        }
    }

private:
    AIProviderConfig(IAIProvider::ProviderType providerType,
                     const QString& ep,
                     const QString& key,
                     const QString& model,
                     int timeoutMs,
                     int retries)
        : type(providerType)
        , endpoint(ep)
        , apiKey(key)
        , modelName(model)
        , timeout(timeoutMs)
        , maxRetries(retries)
    {}
};

#endif // IAIPROVIDER_H
