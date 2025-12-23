#include "LMStudioProvider.h"
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

LMStudioProvider::LMStudioProvider(QObject* parent)
    : IAIProvider(parent)
    , m_networkManager(new QNetworkAccessManager(this))
    , m_serverUrl("http://localhost:1234")
    , m_modelName("llava")
    , m_timeout(120000)  // 120 seconds for local vision models
    , m_maxRetries(3)
{
}

LMStudioProvider::LMStudioProvider(const AIProviderConfig& config, QObject* parent)
    : IAIProvider(parent)
    , m_networkManager(new QNetworkAccessManager(this))
    , m_serverUrl(config.endpoint)
    , m_modelName(config.modelName)
    , m_timeout(config.timeout)
    , m_maxRetries(config.maxRetries)
{
}

LMStudioProvider::~LMStudioProvider()
{
}

void LMStudioProvider::setEndpoint(const QString& endpoint)
{
    m_serverUrl = endpoint;
    if (!m_serverUrl.endsWith("/v1/chat/completions")) {
        if (!m_serverUrl.endsWith("/")) {
            m_serverUrl += "/";
        }
        m_serverUrl += "v1/chat/completions";
    }
}

void LMStudioProvider::analyzeImageForEnhancements(const QString& imagePath)
{
    qDebug() << "LMStudioProvider: Analyzing image:" << imagePath;

    // Encode image to base64
    QString base64Image = ImageEncoder::imageToBase64(imagePath);
    if (base64Image.isEmpty()) {
        emit analysisError("Failed to encode image to base64");
        return;
    }

    QString mimeType = ImageEncoder::getMimeType(imagePath);

    // Build request JSON (OpenAI-compatible format)
    QJsonObject root;
    root["model"] = m_modelName;
    root["max_tokens"] = 1024;
    root["temperature"] = 0.7;

    QJsonArray messages;
    QJsonObject userMessage;
    userMessage["role"] = "user";

    QJsonArray content;

    // Text part
    QJsonObject textPart;
    textPart["type"] = "text";
    textPart["text"] = EnhancementPromptBuilder::generateEnhancementPrompt();
    content.append(textPart);

    // Image part
    QJsonObject imagePart;
    imagePart["type"] = "image_url";
    QJsonObject imageUrl;
    imageUrl["url"] = QString("data:%1;base64,%2").arg(mimeType, base64Image);
    imagePart["image_url"] = imageUrl;
    content.append(imagePart);

    userMessage["content"] = content;
    messages.append(userMessage);
    root["messages"] = messages;

    QJsonDocument doc(root);
    QByteArray jsonData = doc.toJson();

    qDebug() << "LMStudioProvider: Sending request to" << m_serverUrl;

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
            qWarning() << "LMStudioProvider: Failed to parse response:" << parseError.errorString();
            return false;
        }

        QJsonObject responseObj = responseDoc.object();
        QJsonArray choices = responseObj.value("choices").toArray();

        if (choices.isEmpty()) {
            qWarning() << "LMStudioProvider: No choices in response";
            return false;
        }

        QJsonObject firstChoice = choices[0].toObject();
        QJsonObject message = firstChoice.value("message").toObject();
        QString aiResponse = message.value("content").toString();

        qDebug() << "LMStudioProvider: Received response from AI";

        // Parse enhancement suggestions
        ImageEnhancementAnalysis analysis;
        if (!EnhancementResponseParser::parseEnhancementResponse(aiResponse, analysis)) {
            qWarning() << "LMStudioProvider: Failed to parse enhancement response, creating fallback";
            analysis = EnhancementResponseParser::createFallbackAnalysis(aiResponse);
        }

        emit enhancementAnalysisCompleted(analysis);
        return true;
    });

    if (!success) {
        emit analysisError("Failed to get AI enhancement suggestions after retries");
    }
}

bool LMStudioProvider::testConnection()
{
    qDebug() << "LMStudioProvider: Testing connection to" << m_serverUrl;

    // Simple test: try to get models list
    QString modelsUrl = m_serverUrl;
    modelsUrl.replace("/v1/chat/completions", "/v1/models");

    QNetworkRequest request(modelsUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply* reply = m_networkManager->get(request);

    QEventLoop loop;
    QTimer timeoutTimer;
    timeoutTimer.setSingleShot(true);

    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    connect(&timeoutTimer, &QTimer::timeout, &loop, &QEventLoop::quit);

    timeoutTimer.start(5000);  // 5 second timeout for connection test
    loop.exec();

    bool success = false;
    QString message;

    if (!timeoutTimer.isActive()) {
        // Timeout occurred
        message = "Connection timeout. Is LM Studio running?";
        reply->abort();
    } else {
        timeoutTimer.stop();

        if (reply->error() == QNetworkReply::NoError) {
            success = true;
            message = "Connection successful";
        } else {
            message = QString("Connection failed: %1").arg(reply->errorString());
        }
    }

    reply->deleteLater();

    emit connectionTestResult(success, message);
    return success;
}

QStringList LMStudioProvider::getAvailableModels()
{
    // Return default list - LM Studio doesn't have a standard way to list models
    return QStringList{"llava", "bakllava", "llava-1.6", "moondream"};
}

std::pair<bool, QByteArray> LMStudioProvider::performRequestSync(const QByteArray& jsonData)
{
    QString endpoint = m_serverUrl;
    if (!endpoint.endsWith("/v1/chat/completions")) {
        if (!endpoint.endsWith("/")) {
            endpoint += "/";
        }
        endpoint += "v1/chat/completions";
    }

    QNetworkRequest request(endpoint);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

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
        qWarning() << "LMStudioProvider: Request timeout";
        reply->abort();
    } else {
        timeoutTimer.stop();

        if (reply->error() == QNetworkReply::NoError) {
            responseData = reply->readAll();
            success = true;
        } else {
            qWarning() << "LMStudioProvider: Network error:" << reply->errorString();
        }
    }

    reply->deleteLater();

    return {success, responseData};
}
