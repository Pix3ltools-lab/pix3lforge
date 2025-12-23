#include "viewmanager.h"
#include <QLabel>
#include <QScrollArea>
#include <QScrollBar>
#include <QPixmap>
#include <QImage>

ViewManager::ViewManager(QLabel *imageLabel, QScrollArea *scrollArea, QObject *parent)
    : QObject(parent)
    , m_imageLabel(imageLabel)
    , m_scrollArea(scrollArea)
    , m_scaleFactor(1.0)
    , m_fitToWindow(false)
{
}

void ViewManager::displayImage(const QImage &image)
{
    if (image.isNull())
        return;

    m_imageLabel->setPixmap(QPixmap::fromImage(image));
    m_imageLabel->adjustSize();

    updateZoomLimits();
}

void ViewManager::zoomIn()
{
    scaleImage(1.25);
}

void ViewManager::zoomOut()
{
    scaleImage(0.8);
}

void ViewManager::normalSize()
{
    m_imageLabel->adjustSize();
    m_scaleFactor = 1.0;

    emit scaleFactorChanged(m_scaleFactor);
    updateZoomLimits();
}

void ViewManager::setFitToWindow(bool fit)
{
    m_fitToWindow = fit;
    m_scrollArea->setWidgetResizable(fit);

    if (!fit) {
        normalSize();
    }

    emit fitToWindowChanged(fit);
}

void ViewManager::setScaleFactor(double factor)
{
    if (factor > 0 && factor != m_scaleFactor) {
        m_scaleFactor = factor;
        emit scaleFactorChanged(m_scaleFactor);
        updateZoomLimits();
    }
}

void ViewManager::reset()
{
    m_scaleFactor = 1.0;
    m_fitToWindow = false;
    m_scrollArea->setWidgetResizable(false);

    emit scaleFactorChanged(m_scaleFactor);
    emit fitToWindowChanged(m_fitToWindow);
    updateZoomLimits();
}

void ViewManager::scaleImage(double factor)
{
    if (m_imageLabel->pixmap().isNull())
        return;

    m_scaleFactor *= factor;
    m_imageLabel->resize(m_scaleFactor * m_imageLabel->pixmap().size());

    adjustScrollBar(m_scrollArea->horizontalScrollBar(), factor);
    adjustScrollBar(m_scrollArea->verticalScrollBar(), factor);

    emit scaleFactorChanged(m_scaleFactor);
    updateZoomLimits();
}

void ViewManager::adjustScrollBar(QScrollBar *scrollBar, double factor)
{
    scrollBar->setValue(int(factor * scrollBar->value()
                            + ((factor - 1) * scrollBar->pageStep()/2)));
}

void ViewManager::updateZoomLimits()
{
    emit zoomLimitsChanged(canZoomIn(), canZoomOut());
}
