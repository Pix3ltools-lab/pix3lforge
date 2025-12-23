#include "previewmanager.h"
#include "../imageprocessor.h"

PreviewManager::PreviewManager(ImageProcessor *processor, QObject *parent)
    : QObject(parent)
    , m_processor(processor)
    , m_isProcessing(false)
{
}

QImage PreviewManager::generatePreview(const QImage &source, const AdjustmentParameters &params)
{
    if (source.isNull())
        return source;

    return applyAdjustments(source, params);
}

QImage PreviewManager::getOptimizedPreviewSource(const QImage &source, int maxDimension)
{
    if (source.width() <= maxDimension && source.height() <= maxDimension)
        return source; // Already small enough

    // Downscale for preview
    return source.scaled(maxDimension, maxDimension,
                        Qt::KeepAspectRatio, Qt::FastTransformation);
}

void PreviewManager::setProcessing(bool processing)
{
    if (m_isProcessing != processing) {
        m_isProcessing = processing;
        emit processingStateChanged(m_isProcessing);
    }
}

QImage PreviewManager::applyAdjustments(const QImage &source, const AdjustmentParameters &params)
{
    QImage result = source;

    // Apply adjustments in sequence
    if (params.brightness != 0)
        result = m_processor->adjustBrightness(result, params.brightness);

    if (params.contrast != 0)
        result = m_processor->adjustContrast(result, params.contrast);

    if (params.saturation != 0)
        result = m_processor->adjustSaturation(result, params.saturation);

    if (params.hue != 0)
        result = m_processor->adjustHue(result, params.hue);

    if (qAbs(params.gamma - 1.0) > 0.01)
        result = m_processor->adjustGamma(result, params.gamma);

    if (params.temperature != 0)
        result = m_processor->adjustColorTemperature(result, params.temperature);

    if (params.exposure != 0)
        result = m_processor->adjustExposure(result, params.exposure);

    if (params.shadows != 0)
        result = m_processor->adjustShadows(result, params.shadows);

    if (params.highlights != 0)
        result = m_processor->adjustHighlights(result, params.highlights);

    return result;
}
