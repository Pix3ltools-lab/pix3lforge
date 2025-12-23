#ifndef PREVIEWMANAGER_H
#define PREVIEWMANAGER_H

#include <QObject>
#include <QImage>
#include "../model/adjustmentparameters.h"

class ImageProcessor;

class PreviewManager : public QObject
{
    Q_OBJECT

public:
    explicit PreviewManager(ImageProcessor *processor, QObject *parent = nullptr);

    // Generate preview with optimized source
    QImage generatePreview(const QImage &source, const AdjustmentParameters &params);

    // Get optimized preview source (downscaled for performance)
    QImage getOptimizedPreviewSource(const QImage &source, int maxDimension = 1920);

    bool isProcessing() const { return m_isProcessing; }
    void setProcessing(bool processing);

signals:
    void processingStateChanged(bool isProcessing);

private:
    ImageProcessor *m_processor;
    bool m_isProcessing;

    QImage applyAdjustments(const QImage &source, const AdjustmentParameters &params);
};

#endif // PREVIEWMANAGER_H
