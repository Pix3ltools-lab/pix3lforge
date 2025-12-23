#include "OpenAIProvider.h"
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

OpenAIProvider::OpenAIProvider(QObject* parent)
    : IAIProvider(parent)
    , m_networkManager(new QNetworkAccessManager(this))
    , m_endpoint("https://api.openai.com/v1/chat/completions")
    , m_apiKey("")
    , m_modelName("gpt-4o")
    , m_timeout(30000)
    , m_maxRetries(3)
{
}

OpenAIProvider::OpenAIProvider(const AIProviderConfig& config, QObject* parent)
    : IAIProvider(parent)
    , m_networkManager(new QNetworkAccessManager(this))
    , m_endpoint(config.endpoint)
    , m_apiKey(config.apiKey)
    , m_modelName(config.modelName)
    , m_timeout(config.timeout)
    , m_maxRetries(config.maxRetries)
{
}

OpenAIProvider::~OpenAIProvider()
{
}

void OpenAIProvider::analyzeImageForEnhancements(const QString& imagePath)
{
    if (m_apiKey.isEmpty()) {
        emit analysisError("OpenAI API key is not configured");
        return;
    }

    qDebug() << "OpenAIProvider: Analyzing image:" << imagePath;

    // Encode image to base64
    QString base64Image = ImageEncoder::imageToBase64(imagePath);
    if (base64Image.isEmpty()) {
        emit analysisError("Failed to encode image to base64");
        return;
    }

    QString mimeType = ImageEncoder::getMimeType(imagePath);

    // Build request JSON (OpenAI format)
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

    qDebug() << "OpenAIProvider: Sending request to" << m_endpoint;

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
            qWarning() << "OpenAIProvider: Failed to parse response:" << parseError.errorString();
            return false;
        }

        QJsonObject responseObj = responseDoc.object();

        // Check for error in response
        if (responseObj.contains("error")) {
            QJsonObject error = responseObj.value("error").toObject();
            QString errorMsg = error.value("message").toString();
            qWarning() << "OpenAIProvider: API error:" << errorMsg;
            emit analysisError(QString("OpenAI API error: %1").arg(errorMsg));
            return false;
        }

        QJsonArray choices = responseObj.value("choices").toArray();

        if (choices.isEmpty()) {
            qWarning() << "OpenAIProvider: No choices in response";
            return false;
        }

        QJsonObject firstChoice = choices[0].toObject();
        QJsonObject message = firstChoice.value("message").toObject();
        QString aiResponse = message.value("content").toString();

        qDebug() << "OpenAIProvider: Received response from AI";

        // Parse enhancement suggestions
        ImageEnhancementAnalysis analysis;
        if (!EnhancementResponseParser::parseEnhancementResponse(aiResponse, analysis)) {
            qWarning() << "OpenAIProvider: Failed to parse enhancement response, creating fallback";
            analysis = EnhancementResponseParser::createFallbackAnalysis(aiResponse);
        }

        emit enhancementAnalysisCompleted(analysis);
        return true;
    });

    if (!success) {
        emit analysisError("Failed to get AI enhancement suggestions after retries");
    }
}

bool OpenAIProvider::testConnection()
{
    if (m_apiKey.isEmpty()) {
        emit connectionTestResult(false, "API key is not configured");
        return false;
    }

    qDebug() << "OpenAIProvider: Testing connection to" << m_endpoint;

    // Simple test: minimal request with very short prompt
    QJsonObject root;
    root["model"] = m_modelName;
    root["max_tokens"] = 10;

    QJsonArray messages;
    QJsonObject message;
    message["role"] = "user";
    message["content"] = "Hi";
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

QStringList OpenAIProvider::getAvailableModels()
{
    return getRecommendedModels();
}

QStringList OpenAIProvider::getRecommendedModels()
{
    return QStringList{
        "gpt-4o",
        "gpt-4o-mini",
        "gpt-4-turbo"
    };
}

std::pair<bool, QByteArray> OpenAIProvider::performRequestSync(const QByteArray& jsonData)
{
    QNetworkRequest request(m_endpoint);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", QString("Bearer %1").arg(m_apiKey).toUtf8());

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
        qWarning() << "OpenAIProvider: Request timeout";
        reply->abort();
    } else {
        timeoutTimer.stop();

        if (reply->error() == QNetworkReply::NoError) {
            responseData = reply->readAll();
            success = true;
        } else {
            qWarning() << "OpenAIProvider: Network error:" << reply->errorString();
            responseData = reply->readAll();  // May contain error details
        }
    }

    reply->deleteLater();

    return {success, responseData};
}
