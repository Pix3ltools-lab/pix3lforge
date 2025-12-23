#include "imagecommand.h"
#include "../imageprocessor.h"

// Base ImageCommand implementation
ImageCommand::ImageCommand(QImage *targetImage, const QString &text, QUndoCommand *parent)
    : QUndoCommand(text, parent)
    , m_targetImage(targetImage)
    , m_previousImage(*targetImage)
    , m_firstRedo(true)
{
}

void ImageCommand::undo()
{
    if (m_targetImage) {
        *m_targetImage = m_previousImage;
    }
}

void ImageCommand::redo()
{
    if (m_targetImage) {
        if (m_firstRedo) {
            m_newImage = applyOperation(m_previousImage);
            m_firstRedo = false;
        }
        *m_targetImage = m_newImage;
    }
}

// BrightnessCommand
BrightnessCommand::BrightnessCommand(QImage *targetImage, int brightness, QUndoCommand *parent)
    : ImageCommand(targetImage, QObject::tr("Adjust Brightness"), parent)
    , m_brightness(brightness)
{
}

QImage BrightnessCommand::applyOperation(const QImage &image)
{
    ImageProcessor processor;
    return processor.adjustBrightness(image, m_brightness);
}

// ContrastCommand
ContrastCommand::ContrastCommand(QImage *targetImage, int contrast, QUndoCommand *parent)
    : ImageCommand(targetImage, QObject::tr("Adjust Contrast"), parent)
    , m_contrast(contrast)
{
}

QImage ContrastCommand::applyOperation(const QImage &image)
{
    ImageProcessor processor;
    return processor.adjustContrast(image, m_contrast);
}

// SaturationCommand
SaturationCommand::SaturationCommand(QImage *targetImage, int saturation, QUndoCommand *parent)
    : ImageCommand(targetImage, QObject::tr("Adjust Saturation"), parent)
    , m_saturation(saturation)
{
}

QImage SaturationCommand::applyOperation(const QImage &image)
{
    ImageProcessor processor;
    return processor.adjustSaturation(image, m_saturation);
}

// HueCommand
HueCommand::HueCommand(QImage *targetImage, int hue, QUndoCommand *parent)
    : ImageCommand(targetImage, QObject::tr("Adjust Hue"), parent)
    , m_hue(hue)
{
}

QImage HueCommand::applyOperation(const QImage &image)
{
    ImageProcessor processor;
    return processor.adjustHue(image, m_hue);
}

// GammaCommand
GammaCommand::GammaCommand(QImage *targetImage, double gamma, QUndoCommand *parent)
    : ImageCommand(targetImage, QObject::tr("Adjust Gamma"), parent)
    , m_gamma(gamma)
{
}

QImage GammaCommand::applyOperation(const QImage &image)
{
    ImageProcessor processor;
    return processor.adjustGamma(image, m_gamma);
}

// ColorTemperatureCommand
ColorTemperatureCommand::ColorTemperatureCommand(QImage *targetImage, int temperature, QUndoCommand *parent)
    : ImageCommand(targetImage, QObject::tr("Adjust Color Temperature"), parent)
    , m_temperature(temperature)
{
}

QImage ColorTemperatureCommand::applyOperation(const QImage &image)
{
    ImageProcessor processor;
    return processor.adjustColorTemperature(image, m_temperature);
}

// ExposureCommand
ExposureCommand::ExposureCommand(QImage *targetImage, int exposure, QUndoCommand *parent)
    : ImageCommand(targetImage, QObject::tr("Adjust Exposure"), parent)
    , m_exposure(exposure)
{
}

QImage ExposureCommand::applyOperation(const QImage &image)
{
    ImageProcessor processor;
    return processor.adjustExposure(image, m_exposure);
}

// ShadowsCommand
ShadowsCommand::ShadowsCommand(QImage *targetImage, int shadows, QUndoCommand *parent)
    : ImageCommand(targetImage, QObject::tr("Adjust Shadows"), parent)
    , m_shadows(shadows)
{
}

QImage ShadowsCommand::applyOperation(const QImage &image)
{
    ImageProcessor processor;
    return processor.adjustShadows(image, m_shadows);
}

// HighlightsCommand
HighlightsCommand::HighlightsCommand(QImage *targetImage, int highlights, QUndoCommand *parent)
    : ImageCommand(targetImage, QObject::tr("Adjust Highlights"), parent)
    , m_highlights(highlights)
{
}

QImage HighlightsCommand::applyOperation(const QImage &image)
{
    ImageProcessor processor;
    return processor.adjustHighlights(image, m_highlights);
}

// FilterCommand
FilterCommand::FilterCommand(QImage *targetImage, FilterType filterType, QUndoCommand *parent)
    : ImageCommand(targetImage, QString(), parent)
    , m_filterType(filterType)
{
    switch (filterType) {
    case BlackAndWhite:
        setText(QObject::tr("Apply Black & White"));
        break;
    case Sepia:
        setText(QObject::tr("Apply Sepia"));
        break;
    case Vignette:
        setText(QObject::tr("Apply Vignette"));
        break;
    case Sharpen:
        setText(QObject::tr("Apply Sharpen"));
        break;
    case EdgeDetection:
        setText(QObject::tr("Apply Edge Detection"));
        break;
    }
}

QImage FilterCommand::applyOperation(const QImage &image)
{
    ImageProcessor processor;
    switch (m_filterType) {
    case BlackAndWhite:
        return processor.applyBlackAndWhite(image);
    case Sepia:
        return processor.applySepia(image);
    case Vignette:
        return processor.applyVignette(image);
    case Sharpen:
        return processor.applySharpen(image);
    case EdgeDetection:
        return processor.applyEdgeDetection(image);
    default:
        return image;
    }
}

// BlurCommand
BlurCommand::BlurCommand(QImage *targetImage, int radius, QUndoCommand *parent)
    : ImageCommand(targetImage, QObject::tr("Apply Blur"), parent)
    , m_radius(radius)
{
}

QImage BlurCommand::applyOperation(const QImage &image)
{
    ImageProcessor processor;
    return processor.applyBlur(image, m_radius);
}

// RotateCommand
RotateCommand::RotateCommand(QImage *targetImage, int angle, QUndoCommand *parent)
    : ImageCommand(targetImage, QObject::tr("Rotate Image"), parent)
    , m_angle(angle)
{
}

QImage RotateCommand::applyOperation(const QImage &image)
{
    ImageProcessor processor;
    return processor.rotate(image, m_angle);
}

// FlipCommand
FlipCommand::FlipCommand(QImage *targetImage, FlipType flipType, QUndoCommand *parent)
    : ImageCommand(targetImage, QString(), parent)
    , m_flipType(flipType)
{
    setText(flipType == Horizontal ? QObject::tr("Flip Horizontal") : QObject::tr("Flip Vertical"));
}

QImage FlipCommand::applyOperation(const QImage &image)
{
    ImageProcessor processor;
    if (m_flipType == Horizontal) {
        return processor.flipHorizontal(image);
    } else {
        return processor.flipVertical(image);
    }
}

// ResizeCommand
ResizeCommand::ResizeCommand(QImage *targetImage, int width, int height, QUndoCommand *parent)
    : ImageCommand(targetImage, QObject::tr("Resize Image"), parent)
    , m_width(width)
    , m_height(height)
{
}

QImage ResizeCommand::applyOperation(const QImage &image)
{
    ImageProcessor processor;
    return processor.resize(image, m_width, m_height);
}

// CropCommand
CropCommand::CropCommand(QImage *targetImage, int x, int y, int width, int height, QUndoCommand *parent)
    : ImageCommand(targetImage, QObject::tr("Crop Image"), parent)
    , m_x(x)
    , m_y(y)
    , m_width(width)
    , m_height(height)
{
}

QImage CropCommand::applyOperation(const QImage &image)
{
    ImageProcessor processor;
    return processor.crop(image, m_x, m_y, m_width, m_height);
}

// TextWatermarkCommand
TextWatermarkCommand::TextWatermarkCommand(QImage *targetImage, const QString &text, int x, int y, QUndoCommand *parent)
    : ImageCommand(targetImage, QObject::tr("Add Text Watermark"), parent)
    , m_text(text)
    , m_x(x)
    , m_y(y)
{
}

QImage TextWatermarkCommand::applyOperation(const QImage &image)
{
    ImageProcessor processor;
    return processor.addTextWatermark(image, m_text, m_x, m_y);
}

// ImageWatermarkCommand
ImageWatermarkCommand::ImageWatermarkCommand(QImage *targetImage, const QImage &watermark, int x, int y, QUndoCommand *parent)
    : ImageCommand(targetImage, QObject::tr("Add Image Watermark"), parent)
    , m_watermark(watermark)
    , m_x(x)
    , m_y(y)
{
}

QImage ImageWatermarkCommand::applyOperation(const QImage &image)
{
    ImageProcessor processor;
    return processor.addImageWatermark(image, m_watermark, m_x, m_y);
}

// CompoundAdjustmentCommand
CompoundAdjustmentCommand::CompoundAdjustmentCommand(QImage *targetImage, const QString &text)
    : QUndoCommand(text)
    , m_targetImage(targetImage)
{
    // Note: Child commands are added via constructor parameter, not via addAdjustment
    // This is handled in MainWindow::onApplyAdjustments()
}

void CompoundAdjustmentCommand::addAdjustment(ImageCommand *command)
{
    // The command becomes a child - it's already added via parent in constructor
    // No action needed here
}
