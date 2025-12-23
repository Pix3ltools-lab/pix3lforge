#include "EnhancementPromptBuilder.h"

QString EnhancementPromptBuilder::generateEnhancementPrompt()
{
    return R"(Analyze this image and suggest specific enhancements to improve its quality.

Please provide your response in JSON format with the following structure:

{
  "overallAssessment": "Brief overall assessment of the image",
  "technicalAnalysis": "Technical details about exposure, colors, sharpness, etc.",
  "suggestions": [
    {
      "operation": "brightness|contrast|saturation|hue|gamma|temperature|exposure|shadows|highlights|sharpen|blur",
      "value": <numeric value>,
      "reason": "Why this adjustment is recommended",
      "confidence": <0.0 to 1.0>
    }
  ]
}

Guidelines for suggestions:
- brightness: Range -100 to +100 (adjust overall brightness)
- contrast: Range -100 to +100 (increase/decrease contrast)
- saturation: Range -100 to +100 (color intensity)
- hue: Range -180 to +180 (color shift)
- gamma: Range 0.1 to 3.0 (gamma correction)
- temperature: Range -100 to +100 (warm/cool color temperature)
- exposure: Range -100 to +100 (exposure adjustment)
- shadows: Range -100 to +100 (lift/crush shadows)
- highlights: Range -100 to +100 (recover/blow highlights)
- sharpen: Value 0-100 (apply sharpening, 0=none, 100=max)
- blur: Value 0-100 (apply blur, 0=none, 100=max)

Focus on realistic, subtle improvements. Do not suggest extreme adjustments unless truly needed.
Provide 3-7 suggestions maximum, prioritizing the most impactful changes.)";
}

QString EnhancementPromptBuilder::generateAssessmentPrompt()
{
    return "Analyze this image and provide a technical assessment of its quality, "
           "including notes on exposure, color balance, sharpness, and any visible issues. "
           "Keep the response concise (2-3 sentences).";
}
