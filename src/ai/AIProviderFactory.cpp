#include "AIProviderFactory.h"
#include "providers/LMStudioProvider.h"
#include "providers/OpenRouterProvider.h"
#include "providers/OpenAIProvider.h"
#include "providers/AnthropicProvider.h"

IAIProvider* AIProviderFactory::createProvider(const AIProviderConfig& config, QObject* parent)
{
    switch (config.type) {
    case IAIProvider::LMStudio:
        return new LMStudioProvider(config, parent);
    case IAIProvider::OpenRouter:
        return new OpenRouterProvider(config, parent);
    case IAIProvider::OpenAI:
        return new OpenAIProvider(config, parent);
    case IAIProvider::Anthropic:
        return new AnthropicProvider(config, parent);
    default:
        return new LMStudioProvider(config, parent);
    }
}

QString AIProviderFactory::getDefaultEndpoint(IAIProvider::ProviderType type)
{
    return AIProviderConfig::getDefaultConfig(type).endpoint;
}

QString AIProviderFactory::getProviderDescription(IAIProvider::ProviderType type)
{
    switch (type) {
    case IAIProvider::LMStudio:
        return "Run AI models locally on your computer using LM Studio. "
               "No API key required. Requires LM Studio running with a vision-capable model.";
    case IAIProvider::OpenRouter:
        return "Access 100+ vision models through OpenRouter (unified API). "
               "Supports Gemini, Claude, GPT-4, Llama Vision, and more. Requires API key.";
    case IAIProvider::OpenAI:
        return "Use OpenAI's GPT-4o or GPT-4o-mini models for image analysis. "
               "High quality but requires API key and credits.";
    case IAIProvider::Anthropic:
        return "Use Anthropic's Claude 3.5 Sonnet for advanced image understanding. "
               "Excellent vision capabilities. Requires API key and credits.";
    default:
        return "";
    }
}

bool AIProviderFactory::requiresApiKey(IAIProvider::ProviderType type)
{
    return type != IAIProvider::LMStudio;
}

QStringList AIProviderFactory::getModelsForProvider(IAIProvider::ProviderType type)
{
    switch (type) {
    case IAIProvider::LMStudio:
        return QStringList{"google/gemma-3-27b", "llava", "bakllava", "llava-1.6", "moondream"};
    case IAIProvider::OpenRouter:
        // Recommended OpenRouter models
        return QStringList{
            "google/gemini-flash-1.5-8b",
            "google/gemini-pro-1.5",
            "anthropic/claude-3.5-sonnet",
            "openai/gpt-4o",
            "openai/gpt-4o-mini",
            "meta-llama/llama-3.2-90b-vision-instruct",
            "qwen/qwen-2-vl-72b-instruct"
        };
    case IAIProvider::OpenAI:
        return QStringList{"gpt-4o", "gpt-4o-mini", "gpt-4-turbo"};
    case IAIProvider::Anthropic:
        return QStringList{
            "claude-3-5-sonnet-20241022",
            "claude-3-opus-20240229",
            "claude-3-sonnet-20240229",
            "claude-3-haiku-20240307"
        };
    default:
        return QStringList();
    }
}
