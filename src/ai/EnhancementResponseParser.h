#ifndef ENHANCEMENTRESPONSEPARSER_H
#define ENHANCEMENTRESPONSEPARSER_H

#include <QString>
#include <QList>

/**
 * Single enhancement suggestion from AI
 */
struct ImageEnhancementSuggestion {
    QString operation;      // "brightness", "contrast", "saturation", "sharpen", "temperature", etc.
    double value;           // Suggested adjustment value
    QString reason;         // Why this adjustment is recommended
    double confidence;      // AI confidence (0.0 - 1.0)
    bool selected;          // User selection for applying (default: true)

    ImageEnhancementSuggestion()
        : value(0.0)
        , confidence(0.0)
        , selected(true)
    {}
};

/**
 * Complete enhancement analysis from AI
 */
struct ImageEnhancementAnalysis {
    QString overallAssessment;  // General assessment (e.g., "Image is slightly underexposed")
    QList<ImageEnhancementSuggestion> suggestions;
    QString technicalAnalysis;  // Technical details about the image

    ImageEnhancementAnalysis() {}
};

/**
 * Parser for AI enhancement responses
 */
class EnhancementResponseParser
{
public:
    /**
     * Parse structured JSON response from AI into ImageEnhancementAnalysis
     * @param jsonString Raw JSON string from AI response
     * @param analysis Output structure to fill
     * @return true if parsing succeeded
     */
    static bool parseEnhancementResponse(const QString& jsonString, ImageEnhancementAnalysis& analysis);

    /**
     * Extract JSON content from response that may contain extra text
     * @param response Raw response that may have text before/after JSON
     * @return Extracted JSON string
     */
    static QString extractJsonFromResponse(const QString& response);

    /**
     * Create fallback analysis from plain text
     * @param description Plain text description
     * @return ImageEnhancementAnalysis with basic suggestions
     */
    static ImageEnhancementAnalysis createFallbackAnalysis(const QString& description);

    /**
     * Validate that analysis has meaningful data
     */
    static bool isValidAnalysis(const ImageEnhancementAnalysis& analysis);
};

#endif // ENHANCEMENTRESPONSEPARSER_H
