#include "commandfactory.h"
#include <QObject>
#include <cmath>

// Adjustment commands
BrightnessCommand* CommandFactory::createBrightnessCommand(QImage *target, int value, QUndoCommand *parent)
{
    return new BrightnessCommand(target, value, parent);
}

ContrastCommand* CommandFactory::createContrastCommand(QImage *target, int value, QUndoCommand *parent)
{
    return new ContrastCommand(target, value, parent);
}

SaturationCommand* CommandFactory::createSaturationCommand(QImage *target, int value, QUndoCommand *parent)
{
    return new SaturationCommand(target, value, parent);
}

HueCommand* CommandFactory::createHueCommand(QImage *target, int value, QUndoCommand *parent)
{
    return new HueCommand(target, value, parent);
}

GammaCommand* CommandFactory::createGammaCommand(QImage *target, double value, QUndoCommand *parent)
{
    return new GammaCommand(target, value, parent);
}

// Color adjustment commands
ColorTemperatureCommand* CommandFactory::createColorTemperatureCommand(QImage *target, int value, QUndoCommand *parent)
{
    return new ColorTemperatureCommand(target, value, parent);
}

ExposureCommand* CommandFactory::createExposureCommand(QImage *target, int value, QUndoCommand *parent)
{
    return new ExposureCommand(target, value, parent);
}

ShadowsCommand* CommandFactory::createShadowsCommand(QImage *target, int value, QUndoCommand *parent)
{
    return new ShadowsCommand(target, value, parent);
}

HighlightsCommand* CommandFactory::createHighlightsCommand(QImage *target, int value, QUndoCommand *parent)
{
    return new HighlightsCommand(target, value, parent);
}

// Filter commands
FilterCommand* CommandFactory::createFilterCommand(QImage *target, FilterCommand::FilterType type, QUndoCommand *parent)
{
    return new FilterCommand(target, type, parent);
}

BlurCommand* CommandFactory::createBlurCommand(QImage *target, int radius, QUndoCommand *parent)
{
    return new BlurCommand(target, radius, parent);
}

// Transformation commands
RotateCommand* CommandFactory::createRotateCommand(QImage *target, int angle, QUndoCommand *parent)
{
    return new RotateCommand(target, angle, parent);
}

FlipCommand* CommandFactory::createFlipCommand(QImage *target, FlipCommand::FlipType type, QUndoCommand *parent)
{
    return new FlipCommand(target, type, parent);
}

ResizeCommand* CommandFactory::createResizeCommand(QImage *target, int width, int height, QUndoCommand *parent)
{
    return new ResizeCommand(target, width, height, parent);
}

CropCommand* CommandFactory::createCropCommand(QImage *target, int x, int y, int width, int height, QUndoCommand *parent)
{
    return new CropCommand(target, x, y, width, height, parent);
}

// Watermark commands
TextWatermarkCommand* CommandFactory::createTextWatermarkCommand(QImage *target, const QString &text, int x, int y, QUndoCommand *parent)
{
    return new TextWatermarkCommand(target, text, x, y, parent);
}

ImageWatermarkCommand* CommandFactory::createImageWatermarkCommand(QImage *target, const QImage &watermark, int x, int y, QUndoCommand *parent)
{
    return new ImageWatermarkCommand(target, watermark, x, y, parent);
}

// Compound command creation
CompoundAdjustmentCommand* CommandFactory::createCompoundAdjustmentCommand(QImage *target,
                                                                          const AdjustmentParameters &params,
                                                                          const QString &text)
{
    QString commandText = text.isEmpty() ? QObject::tr("Apply Adjustments") : text;
    CompoundAdjustmentCommand *compound = new CompoundAdjustmentCommand(target, commandText);

    // Add individual adjustment commands as children only if they have non-default values
    if (params.brightness != 0)
        new BrightnessCommand(target, params.brightness, compound);

    if (params.contrast != 0)
        new ContrastCommand(target, params.contrast, compound);

    if (params.saturation != 0)
        new SaturationCommand(target, params.saturation, compound);

    if (params.hue != 0)
        new HueCommand(target, params.hue, compound);

    if (std::abs(params.gamma - 1.0) > 0.01)
        new GammaCommand(target, params.gamma, compound);

    if (params.temperature != 0)
        new ColorTemperatureCommand(target, params.temperature, compound);

    if (params.exposure != 0)
        new ExposureCommand(target, params.exposure, compound);

    if (params.shadows != 0)
        new ShadowsCommand(target, params.shadows, compound);

    if (params.highlights != 0)
        new HighlightsCommand(target, params.highlights, compound);

    return compound;
}
