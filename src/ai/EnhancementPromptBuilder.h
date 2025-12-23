#ifndef ENHANCEMENTPROMPTBUILDER_H
#define ENHANCEMENTPROMPTBUILDER_H

#include <QString>

/**
 * Builder for AI prompts specific to image enhancement
 */
class EnhancementPromptBuilder
{
public:
    /**
     * Generate structured prompt for enhancement suggestions
     * Requests JSON response with specific enhancement operations
     */
    static QString generateEnhancementPrompt();

    /**
     * Generate prompt for general assessment only
     */
    static QString generateAssessmentPrompt();
};

#endif // ENHANCEMENTPROMPTBUILDER_H
