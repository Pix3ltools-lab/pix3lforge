#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H

#include <QImage>
#include <QObject>
#include <QVector>

struct AdjustmentParameters;

/**
 * @struct ImageStats
 * @brief Statistical analysis of an image for auto-enhancement
 */
struct ImageStats
{
    double averageBrightness;  // 0-255: Average brightness across all pixels
    double contrast;           // Standard deviation of brightness values
    double saturation;         // 0-1: Average saturation in HSV color space
    int darkPixels;            // Percentage of dark pixels (< 64)
    int brightPixels;          // Percentage of bright pixels (> 192)
};

class ImageProcessor : public QObject
{
    Q_OBJECT

public:
    explicit ImageProcessor(QObject *parent = nullptr);

    // Basic adjustments
    QImage adjustBrightness(const QImage &image, int brightness);
    QImage adjustContrast(const QImage &image, int contrast);
    QImage adjustSaturation(const QImage &image, int saturation);
    QImage adjustHue(const QImage &image, int hue);
    QImage adjustGamma(const QImage &image, double gamma);

    // Color adjustments
    QImage adjustColorTemperature(const QImage &image, int temperature);
    QImage adjustExposure(const QImage &image, int exposure);
    QImage adjustShadows(const QImage &image, int shadows);
    QImage adjustHighlights(const QImage &image, int highlights);

    // Filters
    QImage applyBlackAndWhite(const QImage &image);
    QImage applySepia(const QImage &image);
    QImage applyVignette(const QImage &image);
    QImage applySharpen(const QImage &image);
    QImage applyBlur(const QImage &image, int radius);
    QImage applyGaussianBlur(const QImage &image, int radius);
    QImage applyEdgeDetection(const QImage &image);

    // Transformations
    QImage rotate(const QImage &image, int angle);
    QImage flipHorizontal(const QImage &image);
    QImage flipVertical(const QImage &image);
    QImage resize(const QImage &image, int width, int height);
    QImage crop(const QImage &image, int x, int y, int width, int height);

    // Watermarking
    QImage addTextWatermark(const QImage &image, const QString &text, int x, int y);
    QImage addImageWatermark(const QImage &image, const QImage &watermark, int x, int y);

    // Auto-enhancement
    ImageStats analyzeImage(const QImage &image);
    QVector<int> calculateHistogram(const QImage &image);
    AdjustmentParameters suggestEnhancements(const ImageStats &stats);

private:
    // Helper functions
    QColor adjustColorTemperatureHelper(const QColor &color, int temperature);
    QColor adjustHueHelper(const QColor &color, int hue);
};

#endif // IMAGEPROCESSOR_H