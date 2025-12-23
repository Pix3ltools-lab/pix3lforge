#ifndef COMMANDFACTORY_H
#define COMMANDFACTORY_H

#include <QUndoCommand>
#include <QImage>
#include <QString>
#include "imagecommand.h"
#include "../model/adjustmentparameters.h"

class ImageDocument;
class ImageProcessor;

/**
 * @class CommandFactory
 * @brief Factory for creating image editing commands
 *
 * This class centralizes the creation of all command objects, reducing
 * the responsibility of MainWindow to know about command constructors.
 *
 * Benefits:
 * - Single place to create all commands
 * - Easier to modify command creation logic
 * - Reduces coupling between UI and command classes
 * - Simplifies testing (can inject mock factory)
 *
 * Pattern: Factory Method + Static Factory
 */
class CommandFactory
{
public:
    // Adjustment commands
    static BrightnessCommand* createBrightnessCommand(QImage *target, int value, QUndoCommand *parent = nullptr);
    static ContrastCommand* createContrastCommand(QImage *target, int value, QUndoCommand *parent = nullptr);
    static SaturationCommand* createSaturationCommand(QImage *target, int value, QUndoCommand *parent = nullptr);
    static HueCommand* createHueCommand(QImage *target, int value, QUndoCommand *parent = nullptr);
    static GammaCommand* createGammaCommand(QImage *target, double value, QUndoCommand *parent = nullptr);

    // Color adjustment commands
    static ColorTemperatureCommand* createColorTemperatureCommand(QImage *target, int value, QUndoCommand *parent = nullptr);
    static ExposureCommand* createExposureCommand(QImage *target, int value, QUndoCommand *parent = nullptr);
    static ShadowsCommand* createShadowsCommand(QImage *target, int value, QUndoCommand *parent = nullptr);
    static HighlightsCommand* createHighlightsCommand(QImage *target, int value, QUndoCommand *parent = nullptr);

    // Filter commands
    static FilterCommand* createFilterCommand(QImage *target, FilterCommand::FilterType type, QUndoCommand *parent = nullptr);
    static BlurCommand* createBlurCommand(QImage *target, int radius, QUndoCommand *parent = nullptr);

    // Transformation commands
    static RotateCommand* createRotateCommand(QImage *target, int angle, QUndoCommand *parent = nullptr);
    static FlipCommand* createFlipCommand(QImage *target, FlipCommand::FlipType type, QUndoCommand *parent = nullptr);
    static ResizeCommand* createResizeCommand(QImage *target, int width, int height, QUndoCommand *parent = nullptr);
    static CropCommand* createCropCommand(QImage *target, int x, int y, int width, int height, QUndoCommand *parent = nullptr);

    // Watermark commands
    static TextWatermarkCommand* createTextWatermarkCommand(QImage *target, const QString &text, int x, int y, QUndoCommand *parent = nullptr);
    static ImageWatermarkCommand* createImageWatermarkCommand(QImage *target, const QImage &watermark, int x, int y, QUndoCommand *parent = nullptr);

    // Compound command creation
    static CompoundAdjustmentCommand* createCompoundAdjustmentCommand(QImage *target,
                                                                      const AdjustmentParameters &params,
                                                                      const QString &text = QString());

private:
    // Private constructor - this is a static factory
    CommandFactory() = delete;
};

#endif // COMMANDFACTORY_H
