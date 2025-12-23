#ifndef ADJUSTMENTPARAMETERS_H
#define ADJUSTMENTPARAMETERS_H

/**
 * @struct AdjustmentParameters
 * @brief Value object that encapsulates all image adjustment parameters
 *
 * This struct provides a type-safe way to pass adjustment values between
 * the PropertiesPanel and the image processing commands. It follows the
 * Value Object pattern by being immutable and containing only data.
 *
 * Benefits:
 * - Reduces parameter coupling between PropertiesPanel and commands
 * - Makes it easy to add new adjustment parameters
 * - Provides a single point of truth for adjustment values
 * - Simplifies function signatures
 */
struct AdjustmentParameters
{
    // Basic adjustments
    int brightness = 0;      // Range: -100 to 100
    int contrast = 0;        // Range: -100 to 100
    int saturation = 0;      // Range: -100 to 100
    int hue = 0;             // Range: -180 to 180
    double gamma = 1.0;      // Range: 0.1 to 10.0

    // Color adjustments
    int temperature = 0;     // Range: -100 to 100
    int exposure = 0;        // Range: -100 to 100
    int shadows = 0;         // Range: -100 to 100
    int highlights = 0;      // Range: -100 to 100

    /**
     * @brief Check if any adjustments have been made
     * @return true if any parameter differs from default values
     */
    bool hasAnyAdjustments() const
    {
        return brightness != 0
            || contrast != 0
            || saturation != 0
            || hue != 0
            || qAbs(gamma - 1.0) > 0.01
            || temperature != 0
            || exposure != 0
            || shadows != 0
            || highlights != 0;
    }

    /**
     * @brief Reset all parameters to default values
     */
    void reset()
    {
        brightness = 0;
        contrast = 0;
        saturation = 0;
        hue = 0;
        gamma = 1.0;
        temperature = 0;
        exposure = 0;
        shadows = 0;
        highlights = 0;
    }

    /**
     * @brief Compare two AdjustmentParameters for equality
     */
    bool operator==(const AdjustmentParameters &other) const
    {
        return brightness == other.brightness
            && contrast == other.contrast
            && saturation == other.saturation
            && hue == other.hue
            && qAbs(gamma - other.gamma) < 0.01
            && temperature == other.temperature
            && exposure == other.exposure
            && shadows == other.shadows
            && highlights == other.highlights;
    }

    /**
     * @brief Compare two AdjustmentParameters for inequality
     */
    bool operator!=(const AdjustmentParameters &other) const
    {
        return !(*this == other);
    }
};

#endif // ADJUSTMENTPARAMETERS_H
