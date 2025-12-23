#ifndef IMAGECOMMAND_H
#define IMAGECOMMAND_H

#include <QUndoCommand>
#include <QImage>
#include <functional>

// Base class for all image editing commands
class ImageCommand : public QUndoCommand
{
public:
    ImageCommand(QImage *targetImage, const QString &text, QUndoCommand *parent = nullptr);

    void undo() override;
    void redo() override;

protected:
    // Override this in derived classes to perform the actual operation
    virtual QImage applyOperation(const QImage &image) = 0;

    QImage *m_targetImage;
    QImage m_previousImage;
    QImage m_newImage;
    bool m_firstRedo;
};

// Adjustment commands
class BrightnessCommand : public ImageCommand
{
public:
    BrightnessCommand(QImage *targetImage, int brightness, QUndoCommand *parent = nullptr);

protected:
    QImage applyOperation(const QImage &image) override;

private:
    int m_brightness;
};

class ContrastCommand : public ImageCommand
{
public:
    ContrastCommand(QImage *targetImage, int contrast, QUndoCommand *parent = nullptr);

protected:
    QImage applyOperation(const QImage &image) override;

private:
    int m_contrast;
};

class SaturationCommand : public ImageCommand
{
public:
    SaturationCommand(QImage *targetImage, int saturation, QUndoCommand *parent = nullptr);

protected:
    QImage applyOperation(const QImage &image) override;

private:
    int m_saturation;
};

class HueCommand : public ImageCommand
{
public:
    HueCommand(QImage *targetImage, int hue, QUndoCommand *parent = nullptr);

protected:
    QImage applyOperation(const QImage &image) override;

private:
    int m_hue;
};

class GammaCommand : public ImageCommand
{
public:
    GammaCommand(QImage *targetImage, double gamma, QUndoCommand *parent = nullptr);

protected:
    QImage applyOperation(const QImage &image) override;

private:
    double m_gamma;
};

// Color adjustment commands
class ColorTemperatureCommand : public ImageCommand
{
public:
    ColorTemperatureCommand(QImage *targetImage, int temperature, QUndoCommand *parent = nullptr);

protected:
    QImage applyOperation(const QImage &image) override;

private:
    int m_temperature;
};

class ExposureCommand : public ImageCommand
{
public:
    ExposureCommand(QImage *targetImage, int exposure, QUndoCommand *parent = nullptr);

protected:
    QImage applyOperation(const QImage &image) override;

private:
    int m_exposure;
};

class ShadowsCommand : public ImageCommand
{
public:
    ShadowsCommand(QImage *targetImage, int shadows, QUndoCommand *parent = nullptr);

protected:
    QImage applyOperation(const QImage &image) override;

private:
    int m_shadows;
};

class HighlightsCommand : public ImageCommand
{
public:
    HighlightsCommand(QImage *targetImage, int highlights, QUndoCommand *parent = nullptr);

protected:
    QImage applyOperation(const QImage &image) override;

private:
    int m_highlights;
};

// Filter commands
class FilterCommand : public ImageCommand
{
public:
    enum FilterType {
        BlackAndWhite,
        Sepia,
        Vignette,
        Sharpen,
        EdgeDetection
    };

    FilterCommand(QImage *targetImage, FilterType filterType, QUndoCommand *parent = nullptr);

protected:
    QImage applyOperation(const QImage &image) override;

private:
    FilterType m_filterType;
};

class BlurCommand : public ImageCommand
{
public:
    BlurCommand(QImage *targetImage, int radius, QUndoCommand *parent = nullptr);

protected:
    QImage applyOperation(const QImage &image) override;

private:
    int m_radius;
};

// Transformation commands
class RotateCommand : public ImageCommand
{
public:
    RotateCommand(QImage *targetImage, int angle, QUndoCommand *parent = nullptr);

protected:
    QImage applyOperation(const QImage &image) override;

private:
    int m_angle;
};

class FlipCommand : public ImageCommand
{
public:
    enum FlipType {
        Horizontal,
        Vertical
    };

    FlipCommand(QImage *targetImage, FlipType flipType, QUndoCommand *parent = nullptr);

protected:
    QImage applyOperation(const QImage &image) override;

private:
    FlipType m_flipType;
};

class ResizeCommand : public ImageCommand
{
public:
    ResizeCommand(QImage *targetImage, int width, int height, QUndoCommand *parent = nullptr);

protected:
    QImage applyOperation(const QImage &image) override;

private:
    int m_width;
    int m_height;
};

class CropCommand : public ImageCommand
{
public:
    CropCommand(QImage *targetImage, int x, int y, int width, int height, QUndoCommand *parent = nullptr);

protected:
    QImage applyOperation(const QImage &image) override;

private:
    int m_x;
    int m_y;
    int m_width;
    int m_height;
};

// Watermark commands
class TextWatermarkCommand : public ImageCommand
{
public:
    TextWatermarkCommand(QImage *targetImage, const QString &text, int x, int y, QUndoCommand *parent = nullptr);

protected:
    QImage applyOperation(const QImage &image) override;

private:
    QString m_text;
    int m_x;
    int m_y;
};

class ImageWatermarkCommand : public ImageCommand
{
public:
    ImageWatermarkCommand(QImage *targetImage, const QImage &watermark, int x, int y, QUndoCommand *parent = nullptr);

protected:
    QImage applyOperation(const QImage &image) override;

private:
    QImage m_watermark;
    int m_x;
    int m_y;
};

// Compound command for applying multiple adjustments at once
class CompoundAdjustmentCommand : public QUndoCommand
{
public:
    CompoundAdjustmentCommand(QImage *targetImage, const QString &text = QObject::tr("Adjust Image"));

    void addAdjustment(ImageCommand *command);

private:
    QImage *m_targetImage;
};

#endif // IMAGECOMMAND_H
