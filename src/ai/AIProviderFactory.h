#ifndef AIPROVIDERFACTORY_H
#define AIPROVIDERFACTORY_H

#include "IAIProvider.h"

/**
 * Factory for creating AI providers
 */
class AIProviderFactory
{
public:
    /**
     * Create an AI provider based on config
     */
    static IAIProvider* createProvider(const AIProviderConfig& config, QObject* parent = nullptr);

    /**
     * Get default endpoint for a provider type
     */
    static QString getDefaultEndpoint(IAIProvider::ProviderType type);

    /**
     * Get provider description
     */
    static QString getProviderDescription(IAIProvider::ProviderType type);

    /**
     * Check if provider requires API key
     */
    static bool requiresApiKey(IAIProvider::ProviderType type);

    /**
     * Get recommended models for provider
     */
    static QStringList getModelsForProvider(IAIProvider::ProviderType type);
};

#endif // AIPROVIDERFACTORY_H
