#include "EnhancementResponseParser.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

bool EnhancementResponseParser::parseEnhancementResponse(const QString& jsonString, ImageEnhancementAnalysis& analysis)
{
    // Extract JSON if response contains extra text
    QString cleanJson = extractJsonFromResponse(jsonString);

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(cleanJson.toUtf8(), &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        qWarning() << "EnhancementResponseParser: JSON parse error:" << parseError.errorString();
        return false;
    }

    if (!doc.isObject()) {
        qWarning() << "EnhancementResponseParser: Response is not a JSON object";
        return false;
    }

    QJsonObject root = doc.object();

    // Parse overall assessment
    analysis.overallAssessment = root.value("overallAssessment").toString();

    // Parse technical analysis
    analysis.technicalAnalysis = root.value("technicalAnalysis").toString();

    // Parse suggestions array
    QJsonArray suggestionsArray = root.value("suggestions").toArray();
    analysis.suggestions.clear();

    for (const QJsonValue& value : suggestionsArray) {
        if (!value.isObject()) {
            continue;
        }

        QJsonObject suggestionObj = value.toObject();
        ImageEnhancementSuggestion suggestion;

        suggestion.operation = suggestionObj.value("operation").toString();
        suggestion.value = suggestionObj.value("value").toDouble();
        suggestion.reason = suggestionObj.value("reason").toString();
        suggestion.confidence = suggestionObj.value("confidence").toDouble();
        suggestion.selected = true;  // Default: all suggestions selected

        // Validate suggestion has required fields
        if (!suggestion.operation.isEmpty() && suggestion.value != 0.0) {
            analysis.suggestions.append(suggestion);
        }
    }

    return isValidAnalysis(analysis);
}

QString EnhancementResponseParser::extractJsonFromResponse(const QString& response)
{
    // Find first { and last }
    int startIdx = response.indexOf('{');
    int endIdx = response.lastIndexOf('}');

    if (startIdx == -1 || endIdx == -1 || startIdx >= endIdx) {
        // No JSON object found, return original
        return response;
    }

    return response.mid(startIdx, endIdx - startIdx + 1);
}

ImageEnhancementAnalysis EnhancementResponseParser::createFallbackAnalysis(const QString& description)
{
    ImageEnhancementAnalysis analysis;
    analysis.overallAssessment = description.isEmpty()
        ? "Unable to analyze image automatically."
        : description;

    analysis.technicalAnalysis = "AI analysis failed. Please adjust manually.";

    // Create basic suggestions as fallback
    ImageEnhancementSuggestion brightnessSuggestion;
    brightnessSuggestion.operation = "brightness";
    brightnessSuggestion.value = 10;
    brightnessSuggestion.reason = "Default brightness adjustment";
    brightnessSuggestion.confidence = 0.5;
    brightnessSuggestion.selected = false;  // Don't auto-select fallback suggestions

    ImageEnhancementSuggestion contrastSuggestion;
    contrastSuggestion.operation = "contrast";
    contrastSuggestion.value = 10;
    contrastSuggestion.reason = "Default contrast adjustment";
    contrastSuggestion.confidence = 0.5;
    contrastSuggestion.selected = false;

    analysis.suggestions.append(brightnessSuggestion);
    analysis.suggestions.append(contrastSuggestion);

    return analysis;
}

bool EnhancementResponseParser::isValidAnalysis(const ImageEnhancementAnalysis& analysis)
{
    // Analysis is valid if it has at least one suggestion
    // or has meaningful assessment/technical analysis
    return !analysis.suggestions.isEmpty()
        || !analysis.overallAssessment.isEmpty()
        || !analysis.technicalAnalysis.isEmpty();
}
