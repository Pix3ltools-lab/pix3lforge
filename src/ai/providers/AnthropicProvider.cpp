#include "AnthropicProvider.h"
#include "../ImageEncoder.h"
#include "../EnhancementPromptBuilder.h"
#include "../EnhancementResponseParser.h"
#include "../RetryPolicy.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QEventLoop>
#include <QTimer>
#include <QDebug>

AnthropicProvider::AnthropicProvider(QObject* parent)
    : IAIProvider(parent)
    , m_networkManager(new QNetworkAccessManager(this))
    , m_endpoint("https://api.anthropic.com/v1/messages")
    , m_apiKey("")
    , m_modelName("claude-3-5-sonnet-20241022")
    , m_timeout(30000)
    , m_maxRetries(3)
{
}

AnthropicProvider::AnthropicProvider(const AIProviderConfig& config, QObject* parent)
    : IAIProvider(parent)
    , m_networkManager(new QNetworkAccessManager(this))
    , m_endpoint(config.endpoint)
    , m_apiKey(config.apiKey)
    , m_modelName(config.modelName)
    , m_timeout(config.timeout)
    , m_maxRetries(config.maxRetries)
{
}

AnthropicProvider::~AnthropicProvider()
{
}

void AnthropicProvider::analyzeImageForEnhancements(const QString& imagePath)
{
    if (m_apiKey.isEmpty()) {
        emit analysisError("Anthropic API key is not configured");
        return;
    }

    qDebug() << "AnthropicProvider: Analyzing image:" << imagePath;

    // Encode image to base64
    QString base64Image = ImageEncoder::imageToBase64(imagePath);
    if (base64Image.isEmpty()) {
        emit analysisError("Failed to encode image to base64");
        return;
    }

    QString mimeType = ImageEncoder::getMimeType(imagePath);

    // Build request JSON (Anthropic-specific format)
    QJsonObject root;
    root["model"] = m_modelName;
    root["max_tokens"] = 1024;

    QJsonArray messages;
    QJsonObject userMessage;
    userMessage["role"] = "user";

    QJsonArray content;

    // Image part (comes BEFORE text in Anthropic format)
    QJsonObject imagePart;
    imagePart["type"] = "image";
    QJsonObject imageSource;
    imageSource["type"] = "base64";
    imageSource["media_type"] = mimeType;
    imageSource["data"] = base64Image;
    imagePart["source"] = imageSource;
    content.append(imagePart);

    // Text part
    QJsonObject textPart;
    textPart["type"] = "text";
    textPart["text"] = EnhancementPromptBuilder::generateEnhancementPrompt();
    content.append(textPart);

    userMessage["content"] = content;
    messages.append(userMessage);
    root["messages"] = messages;

    QJsonDocument doc(root);
    QByteArray jsonData = doc.toJson();

    qDebug() << "AnthropicProvider: Sending request to" << m_endpoint;

    // Perform request with retry policy
    RetryPolicy retryPolicy(m_maxRetries, 1000, 5000);
    bool success = retryPolicy.execute([this, &jsonData]() {
        auto [requestSuccess, responseData] = performRequestSync(jsonData);
        if (!requestSuccess) {
            return false;
        }

        // Parse response
        QJsonParseError parseError;
        QJsonDocument responseDoc = QJsonDocument::fromJson(responseData, &parseError);

        if (parseError.error != QJsonParseError::NoError) {
            qWarning() << "AnthropicProvider: Failed to parse response:" << parseError.errorString();
            return false;
        }

        QJsonObject responseObj = responseDoc.object();

        // Check for error in response
        if (responseObj.contains("error")) {
            QJsonObject error = responseObj.value("error").toObject();
            QString errorMsg = error.value("message").toString();
            qWarning() << "AnthropicProvider: API error:" << errorMsg;
            emit analysisError(QString("Anthropic API error: %1").arg(errorMsg));
            return false;
        }

        // Anthropic response format: content is an array of content blocks
        QJsonArray contentArray = responseObj.value("content").toArray();

        if (contentArray.isEmpty()) {
            qWarning() << "AnthropicProvider: No content in response";
            return false;
        }

        // Extract text from first content block
        QJsonObject firstContent = contentArray[0].toObject();
        QString aiResponse = firstContent.value("text").toString();

        qDebug() << "AnthropicProvider: Received response from AI";

        // Parse enhancement suggestions
        ImageEnhancementAnalysis analysis;
        if (!EnhancementResponseParser::parseEnhancementResponse(aiResponse, analysis)) {
            qWarning() << "AnthropicProvider: Failed to parse enhancement response, creating fallback";
            analysis = EnhancementResponseParser::createFallbackAnalysis(aiResponse);
        }

        emit enhancementAnalysisCompleted(analysis);
        return true;
    });

    if (!success) {
        emit analysisError("Failed to get AI enhancement suggestions after retries");
    }
}

bool AnthropicProvider::testConnection()
{
    if (m_apiKey.isEmpty()) {
        emit connectionTestResult(false, "API key is not configured");
        return false;
    }

    qDebug() << "AnthropicProvider: Testing connection to" << m_endpoint;

    // Simple test: minimal request with very short prompt
    QJsonObject root;
    root["model"] = m_modelName;
    root["max_tokens"] = 10;

    QJsonArray messages;
    QJsonObject message;
    message["role"] = "user";

    QJsonArray content;
    QJsonObject textPart;
    textPart["type"] = "text";
    textPart["text"] = "Hi";
    content.append(textPart);

    message["content"] = content;
    messages.append(message);
    root["messages"] = messages;

    QJsonDocument doc(root);
    QByteArray jsonData = doc.toJson();

    auto [success, responseData] = performRequestSync(jsonData);

    QString message_text;
    if (success) {
        // Check if response is valid
        QJsonParseError parseError;
        QJsonDocument responseDoc = QJsonDocument::fromJson(responseData, &parseError);

        if (parseError.error == QJsonParseError::NoError) {
            QJsonObject responseObj = responseDoc.object();
            if (responseObj.contains("error")) {
                QJsonObject error = responseObj.value("error").toObject();
                message_text = QString("API error: %1").arg(error.value("message").toString());
                success = false;
            } else {
                message_text = "Connection successful";
            }
        } else {
            message_text = "Invalid response from server";
            success = false;
        }
    } else {
        message_text = "Connection failed. Check API key and internet connection.";
    }

    emit connectionTestResult(success, message_text);
    return success;
}

QStringList AnthropicProvider::getAvailableModels()
{
    return getRecommendedModels();
}

QStringList AnthropicProvider::getRecommendedModels()
{
    return QStringList{
        "claude-3-5-sonnet-20241022",
        "claude-3-opus-20240229",
        "claude-3-sonnet-20240229",
        "claude-3-haiku-20240307"
    };
}

std::pair<bool, QByteArray> AnthropicProvider::performRequestSync(const QByteArray& jsonData)
{
    QNetworkRequest request(m_endpoint);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    // Anthropic uses x-api-key header, not Authorization: Bearer
    request.setRawHeader("x-api-key", m_apiKey.toUtf8());
    request.setRawHeader("anthropic-version", "2023-06-01");

    QNetworkReply* reply = m_networkManager->post(request, jsonData);

    QEventLoop loop;
    QTimer timeoutTimer;
    timeoutTimer.setSingleShot(true);

    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    connect(&timeoutTimer, &QTimer::timeout, &loop, &QEventLoop::quit);

    timeoutTimer.start(m_timeout);
    loop.exec();

    bool success = false;
    QByteArray responseData;

    if (!timeoutTimer.isActive()) {
        // Timeout occurred
        qWarning() << "AnthropicProvider: Request timeout";
        reply->abort();
    } else {
        timeoutTimer.stop();

        if (reply->error() == QNetworkReply::NoError) {
            responseData = reply->readAll();
            success = true;
        } else {
            qWarning() << "AnthropicProvider: Network error:" << reply->errorString();
            responseData = reply->readAll();  // May contain error details
        }
    }

    reply->deleteLater();

    return {success, responseData};
}
