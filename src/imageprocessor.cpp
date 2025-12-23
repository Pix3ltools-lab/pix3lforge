#include "imageprocessor.h"
#include "model/adjustmentparameters.h"
#include "logging/logger.h"
#include <QColor>
#include <QPainter>
#include <QtMath>

ImageProcessor::ImageProcessor(QObject *parent)
    : QObject(parent)
{
}

// Basic adjustments
QImage ImageProcessor::adjustBrightness(const QImage &image, int brightness)
{
    if (image.isNull())
        return image;

    QImage result = image.copy();
    brightness = qBound(-100, brightness, 100);

    for (int y = 0; y < result.height(); ++y) {
        for (int x = 0; x < result.width(); ++x) {
            QColor color = result.pixelColor(x, y);
            int r = qBound(0, color.red() + brightness, 255);
            int g = qBound(0, color.green() + brightness, 255);
            int b = qBound(0, color.blue() + brightness, 255);
            result.setPixelColor(x, y, QColor(r, g, b, color.alpha()));
        }
    }

    return result;
}

QImage ImageProcessor::adjustContrast(const QImage &image, int contrast)
{
    if (image.isNull())
        return image;

    QImage result = image.copy();
    contrast = qBound(-100, contrast, 100);
    double factor = (259.0 * (contrast + 255)) / (255.0 * (259 - contrast));

    for (int y = 0; y < result.height(); ++y) {
        for (int x = 0; x < result.width(); ++x) {
            QColor color = result.pixelColor(x, y);
            int r = qBound(0, static_cast<int>(factor * (color.red() - 128) + 128), 255);
            int g = qBound(0, static_cast<int>(factor * (color.green() - 128) + 128), 255);
            int b = qBound(0, static_cast<int>(factor * (color.blue() - 128) + 128), 255);
            result.setPixelColor(x, y, QColor(r, g, b, color.alpha()));
        }
    }

    return result;
}

QImage ImageProcessor::adjustSaturation(const QImage &image, int saturation)
{
    if (image.isNull())
        return image;

    QImage result = image.copy();
    saturation = qBound(-100, saturation, 100);
    double factor = 1.0 + saturation / 100.0;

    for (int y = 0; y < result.height(); ++y) {
        for (int x = 0; x < result.width(); ++x) {
            QColor color = result.pixelColor(x, y);
            QColor hsvColor = color.toHsv();
            int s = qBound(0, static_cast<int>(hsvColor.saturation() * factor), 255);
            hsvColor.setHsv(hsvColor.hue(), s, hsvColor.value(), hsvColor.alpha());
            result.setPixelColor(x, y, hsvColor.toRgb());
        }
    }

    return result;
}

QImage ImageProcessor::adjustHue(const QImage &image, int hue)
{
    if (image.isNull())
        return image;

    QImage result = image.copy();
    hue = qBound(-180, hue, 180);

    for (int y = 0; y < result.height(); ++y) {
        for (int x = 0; x < result.width(); ++x) {
            QColor color = result.pixelColor(x, y);
            QColor hsvColor = color.toHsv();
            int h = (hsvColor.hue() + hue) % 360;
            if (h < 0)
                h += 360;
            hsvColor.setHsv(h, hsvColor.saturation(), hsvColor.value(), hsvColor.alpha());
            result.setPixelColor(x, y, hsvColor.toRgb());
        }
    }

    return result;
}

QImage ImageProcessor::adjustGamma(const QImage &image, double gamma)
{
    if (image.isNull())
        return image;

    QImage result = image.copy();
    gamma = qBound(0.1, gamma, 10.0);

    for (int y = 0; y < result.height(); ++y) {
        for (int x = 0; x < result.width(); ++x) {
            QColor color = result.pixelColor(x, y);
            int r = qBound(0, static_cast<int>(255 * qPow(color.red() / 255.0, 1.0 / gamma)), 255);
            int g = qBound(0, static_cast<int>(255 * qPow(color.green() / 255.0, 1.0 / gamma)), 255);
            int b = qBound(0, static_cast<int>(255 * qPow(color.blue() / 255.0, 1.0 / gamma)), 255);
            result.setPixelColor(x, y, QColor(r, g, b, color.alpha()));
        }
    }

    return result;
}

// Color adjustments
QImage ImageProcessor::adjustColorTemperature(const QImage &image, int temperature)
{
    if (image.isNull())
        return image;

    QImage result = image.copy();
    temperature = qBound(-100, temperature, 100);

    for (int y = 0; y < result.height(); ++y) {
        for (int x = 0; x < result.width(); ++x) {
            QColor color = result.pixelColor(x, y);
            QColor adjustedColor = adjustColorTemperatureHelper(color, temperature);
            result.setPixelColor(x, y, adjustedColor);
        }
    }

    return result;
}

QColor ImageProcessor::adjustColorTemperatureHelper(const QColor &color, int temperature)
{
    QColor result = color;
    if (temperature > 0) {
        // Warmer (more red/yellow)
        int r = result.red();
        int g = qBound(0, result.green() - temperature / 2, 255);
        int b = qBound(0, result.blue() - temperature, 255);
        result.setRgb(r, g, b, result.alpha());
    } else if (temperature < 0) {
        // Cooler (more blue)
        int r = qBound(0, result.red() + temperature, 255);
        int g = qBound(0, result.green() + temperature / 2, 255);
        int b = result.blue();
        result.setRgb(r, g, b, result.alpha());
    }
    return result;
}

QImage ImageProcessor::adjustExposure(const QImage &image, int exposure)
{
    if (image.isNull())
        return image;

    QImage result = image.copy();
    exposure = qBound(-100, exposure, 100);
    double factor = qPow(2.0, exposure / 50.0);

    for (int y = 0; y < result.height(); ++y) {
        for (int x = 0; x < result.width(); ++x) {
            QColor color = result.pixelColor(x, y);
            int r = qBound(0, static_cast<int>(color.red() * factor), 255);
            int g = qBound(0, static_cast<int>(color.green() * factor), 255);
            int b = qBound(0, static_cast<int>(color.blue() * factor), 255);
            result.setPixelColor(x, y, QColor(r, g, b, color.alpha()));
        }
    }

    return result;
}

QImage ImageProcessor::adjustShadows(const QImage &image, int shadows)
{
    if (image.isNull())
        return image;

    QImage result = image.copy();
    shadows = qBound(-100, shadows, 100);
    double factor = shadows / 100.0;

    for (int y = 0; y < result.height(); ++y) {
        for (int x = 0; x < result.width(); ++x) {
            QColor color = result.pixelColor(x, y);
            double luminance = (0.299 * color.red() + 0.587 * color.green() + 0.114 * color.blue()) / 255.0;
            if (luminance < 0.5) { // Shadow areas
                double shadowFactor = 1.0 + factor * (1.0 - luminance * 2.0);
                int r = qBound(0, static_cast<int>(color.red() * shadowFactor), 255);
                int g = qBound(0, static_cast<int>(color.green() * shadowFactor), 255);
                int b = qBound(0, static_cast<int>(color.blue() * shadowFactor), 255);
                result.setPixelColor(x, y, QColor(r, g, b, color.alpha()));
            }
        }
    }

    return result;
}

QImage ImageProcessor::adjustHighlights(const QImage &image, int highlights)
{
    if (image.isNull())
        return image;

    QImage result = image.copy();
    highlights = qBound(-100, highlights, 100);
    double factor = highlights / 100.0;

    for (int y = 0; y < result.height(); ++y) {
        for (int x = 0; x < result.width(); ++x) {
            QColor color = result.pixelColor(x, y);
            double luminance = (0.299 * color.red() + 0.587 * color.green() + 0.114 * color.blue()) / 255.0;
            if (luminance > 0.5) { // Highlight areas
                double highlightFactor = 1.0 - factor * (luminance * 2.0 - 1.0);
                int r = qBound(0, static_cast<int>(color.red() * highlightFactor), 255);
                int g = qBound(0, static_cast<int>(color.green() * highlightFactor), 255);
                int b = qBound(0, static_cast<int>(color.blue() * highlightFactor), 255);
                result.setPixelColor(x, y, QColor(r, g, b, color.alpha()));
            }
        }
    }

    return result;
}

// Filters
QImage ImageProcessor::applyBlackAndWhite(const QImage &image)
{
    if (image.isNull())
        return image;

    QImage result = image.convertToFormat(QImage::Format_Grayscale8);
    return result;
}

QImage ImageProcessor::applySepia(const QImage &image)
{
    if (image.isNull())
        return image;

    QImage result = image.copy();

    for (int y = 0; y < result.height(); ++y) {
        for (int x = 0; x < result.width(); ++x) {
            QColor color = result.pixelColor(x, y);
            int tr = qBound(0, static_cast<int>(0.393 * color.red() + 0.769 * color.green() + 0.189 * color.blue()), 255);
            int tg = qBound(0, static_cast<int>(0.349 * color.red() + 0.686 * color.green() + 0.168 * color.blue()), 255);
            int tb = qBound(0, static_cast<int>(0.272 * color.red() + 0.534 * color.green() + 0.131 * color.blue()), 255);
            result.setPixelColor(x, y, QColor(tr, tg, tb, color.alpha()));
        }
    }

    return result;
}

QImage ImageProcessor::applyVignette(const QImage &image)
{
    if (image.isNull())
        return image;

    QImage result = image.copy();
    int centerX = result.width() / 2;
    int centerY = result.height() / 2;
    double radius = qMin(result.width(), result.height()) / 2.0;

    for (int y = 0; y < result.height(); ++y) {
        for (int x = 0; x < result.width(); ++x) {
            double dx = x - centerX;
            double dy = y - centerY;
            double distance = qSqrt(dx * dx + dy * dy);
            double factor = qBound(0.0, 1.0 - (distance / radius), 1.0);

            QColor color = result.pixelColor(x, y);
            int r = static_cast<int>(color.red() * factor);
            int g = static_cast<int>(color.green() * factor);
            int b = static_cast<int>(color.blue() * factor);
            result.setPixelColor(x, y, QColor(r, g, b, color.alpha()));
        }
    }

    return result;
}

QImage ImageProcessor::applySharpen(const QImage &image)
{
    if (image.isNull())
        return image;

    // Create a copy for reading (source) and one for writing (result)
    QImage source = image.copy();
    QImage result = image.copy();

    // Simple 3x3 sharpening kernel
    int kernel[3][3] = {{0, -1, 0}, {-1, 5, -1}, {0, -1, 0}};

    for (int y = 1; y < result.height() - 1; ++y) {
        for (int x = 1; x < result.width() - 1; ++x) {
            int r = 0, g = 0, b = 0;

            for (int ky = -1; ky <= 1; ++ky) {
                for (int kx = -1; kx <= 1; ++kx) {
                    // Read from source (unmodified), not from result!
                    QColor color = source.pixelColor(x + kx, y + ky);
                    r += color.red() * kernel[ky + 1][kx + 1];
                    g += color.green() * kernel[ky + 1][kx + 1];
                    b += color.blue() * kernel[ky + 1][kx + 1];
                }
            }

            r = qBound(0, r, 255);
            g = qBound(0, g, 255);
            b = qBound(0, b, 255);
            // Keep original alpha channel
            result.setPixelColor(x, y, QColor(r, g, b, source.pixelColor(x, y).alpha()));
        }
    }

    return result;
}

QImage ImageProcessor::applyBlur(const QImage &image, int radius)
{
    if (image.isNull()) {
        LOG_WARNING("applyBlur called with null image");
        return image;
    }

    // Log solo per blur pesanti (immagini grandi)
    if (image.width() * image.height() > 4000000) { // > 4 megapixel
        LOG_DEBUG(QString("Processing blur on large image: %1x%2, radius=%3")
                  .arg(image.width()).arg(image.height()).arg(radius));
    }

    return applyGaussianBlur(image, radius);
}

QImage ImageProcessor::applyGaussianBlur(const QImage &image, int radius)
{
    if (image.isNull())
        return image;

    radius = qBound(1, radius, 10);
    QImage result = image.copy();

    // Horizontal pass
    for (int y = 0; y < result.height(); ++y) {
        for (int x = 0; x < result.width(); ++x) {
            int r = 0, g = 0, b = 0, a = 0, count = 0;

            for (int kx = -radius; kx <= radius; ++kx) {
                int nx = qBound(0, x + kx, result.width() - 1);
                QColor color = result.pixelColor(nx, y);
                r += color.red();
                g += color.green();
                b += color.blue();
                a += color.alpha();
                count++;
            }

            r /= count;
            g /= count;
            b /= count;
            a /= count;
            result.setPixelColor(x, y, QColor(r, g, b, a));
        }
    }

    // Vertical pass
    QImage temp = result.copy();
    for (int y = 0; y < temp.height(); ++y) {
        for (int x = 0; x < temp.width(); ++x) {
            int r = 0, g = 0, b = 0, a = 0, count = 0;

            for (int ky = -radius; ky <= radius; ++ky) {
                int ny = qBound(0, y + ky, temp.height() - 1);
                QColor color = temp.pixelColor(x, ny);
                r += color.red();
                g += color.green();
                b += color.blue();
                a += color.alpha();
                count++;
            }

            r /= count;
            g /= count;
            b /= count;
            a /= count;
            result.setPixelColor(x, y, QColor(r, g, b, a));
        }
    }

    return result;
}

QImage ImageProcessor::applyEdgeDetection(const QImage &image)
{
    if (image.isNull())
        return image;

    QImage result = image.convertToFormat(QImage::Format_Grayscale8);
    // Simple Sobel edge detection
    int Gx[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
    int Gy[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};

    for (int y = 1; y < result.height() - 1; ++y) {
        for (int x = 1; x < result.width() - 1; ++x) {
            int gx = 0, gy = 0;

            for (int ky = -1; ky <= 1; ++ky) {
                for (int kx = -1; kx <= 1; ++kx) {
                    int intensity = qGray(result.pixel(x + kx, y + ky));
                    gx += intensity * Gx[ky + 1][kx + 1];
                    gy += intensity * Gy[ky + 1][kx + 1];
                }
            }

            int magnitude = qBound(0, static_cast<int>(qSqrt(gx * gx + gy * gy) / 4.0), 255);
            result.setPixelColor(x, y, QColor(magnitude, magnitude, magnitude));
        }
    }

    return result;
}

// Transformations
QImage ImageProcessor::rotate(const QImage &image, int angle)
{
    if (image.isNull())
        return image;

    QTransform transform;
    transform.rotate(angle);
    return image.transformed(transform);
}

QImage ImageProcessor::flipHorizontal(const QImage &image)
{
    if (image.isNull())
        return image;

    return image.flipped(Qt::Horizontal);
}

QImage ImageProcessor::flipVertical(const QImage &image)
{
    if (image.isNull())
        return image;

    return image.flipped(Qt::Vertical);
}

QImage ImageProcessor::resize(const QImage &image, int width, int height)
{
    if (image.isNull())
        return image;

    return image.scaled(width, height, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
}

QImage ImageProcessor::crop(const QImage &image, int x, int y, int width, int height)
{
    if (image.isNull())
        return image;

    return image.copy(x, y, width, height);
}

// Watermarking
QImage ImageProcessor::addTextWatermark(const QImage &image, const QString &text, int x, int y)
{
    if (image.isNull() || text.isEmpty())
        return image;

    QImage result = image.copy();
    QPainter painter(&result);
    painter.setPen(QColor(255, 255, 255, 128)); // Semi-transparent white
    painter.setFont(QFont("Arial", 20));
    painter.drawText(x, y, text);
    painter.end();

    return result;
}

QImage ImageProcessor::addImageWatermark(const QImage &image, const QImage &watermark, int x, int y)
{
    if (image.isNull() || watermark.isNull())
        return image;

    QImage result = image.copy();
    QPainter painter(&result);
    painter.setOpacity(0.5); // Semi-transparent
    painter.drawImage(x, y, watermark);
    painter.end();

    return result;
}

// Auto-enhancement
ImageStats ImageProcessor::analyzeImage(const QImage &image)
{
    ImageStats stats;
    stats.averageBrightness = 0;
    stats.contrast = 0;
    stats.saturation = 0;
    stats.darkPixels = 0;
    stats.brightPixels = 0;

    if (image.isNull())
        return stats;

    // Calculate brightness, saturation, and count dark/bright pixels
    double totalBrightness = 0;
    double totalSaturation = 0;
    int darkCount = 0;
    int brightCount = 0;
    int totalPixels = image.width() * image.height();

    QVector<double> brightnessValues;
    brightnessValues.reserve(totalPixels);

    for (int y = 0; y < image.height(); ++y) {
        for (int x = 0; x < image.width(); ++x) {
            QColor color = image.pixelColor(x, y);

            // Calculate brightness (luminance)
            double brightness = 0.299 * color.red() + 0.587 * color.green() + 0.114 * color.blue();
            totalBrightness += brightness;
            brightnessValues.append(brightness);

            // Count dark and bright pixels
            if (brightness < 64) darkCount++;
            if (brightness > 192) brightCount++;

            // Calculate saturation
            QColor hsvColor = color.toHsv();
            totalSaturation += hsvColor.saturationF();
        }
    }

    // Calculate average brightness
    stats.averageBrightness = totalBrightness / totalPixels;

    // Calculate contrast (standard deviation of brightness)
    double variance = 0;
    for (double brightness : brightnessValues) {
        double diff = brightness - stats.averageBrightness;
        variance += diff * diff;
    }
    stats.contrast = qSqrt(variance / totalPixels);

    // Calculate average saturation
    stats.saturation = totalSaturation / totalPixels;

    // Calculate percentages
    stats.darkPixels = (darkCount * 100) / totalPixels;
    stats.brightPixels = (brightCount * 100) / totalPixels;

    return stats;
}

QVector<int> ImageProcessor::calculateHistogram(const QImage &image)
{
    QVector<int> histogram(256, 0);

    if (image.isNull())
        return histogram;

    for (int y = 0; y < image.height(); ++y) {
        for (int x = 0; x < image.width(); ++x) {
            QColor color = image.pixelColor(x, y);
            // Calculate luminance
            int brightness = static_cast<int>(0.299 * color.red() + 0.587 * color.green() + 0.114 * color.blue());
            histogram[brightness]++;
        }
    }

    return histogram;
}

AdjustmentParameters ImageProcessor::suggestEnhancements(const ImageStats &stats)
{
    AdjustmentParameters params;

    // Brightness adjustment
    // Target average brightness: 115-140 (slightly below middle for natural look)
    if (stats.averageBrightness < 100) {
        // Image is too dark
        params.brightness = static_cast<int>((115 - stats.averageBrightness) * 0.6);
        params.brightness = qBound(-100, params.brightness, 100);
    } else if (stats.averageBrightness > 155) {
        // Image is too bright
        params.brightness = static_cast<int>((140 - stats.averageBrightness) * 0.6);
        params.brightness = qBound(-100, params.brightness, 100);
    }

    // Contrast adjustment
    // Target contrast (std dev): 45-65 for good dynamic range
    if (stats.contrast < 40) {
        // Low contrast - increase it
        params.contrast = static_cast<int>((50 - stats.contrast) * 1.5);
        params.contrast = qBound(0, params.contrast, 50);
    } else if (stats.contrast > 75) {
        // Too much contrast - reduce slightly
        params.contrast = static_cast<int>((65 - stats.contrast) * 0.5);
        params.contrast = qBound(-30, params.contrast, 0);
    }

    // Saturation adjustment
    // Target saturation: 0.35-0.55 for vibrant but natural colors
    if (stats.saturation < 0.25) {
        // Desaturated image
        params.saturation = static_cast<int>((0.40 - stats.saturation) * 150);
        params.saturation = qBound(0, params.saturation, 40);
    } else if (stats.saturation > 0.65) {
        // Oversaturated
        params.saturation = static_cast<int>((0.50 - stats.saturation) * 100);
        params.saturation = qBound(-30, params.saturation, 0);
    }

    // Additional adjustments based on dark/bright pixels
    if (stats.darkPixels > 40) {
        // Too many dark pixels - lift shadows
        params.shadows = qBound(0, 20 + params.brightness / 2, 50);
    }

    if (stats.brightPixels > 30) {
        // Too many bright pixels - reduce highlights
        params.highlights = qBound(-50, -15 + params.brightness / 2, 0);
    }

    return params;
}