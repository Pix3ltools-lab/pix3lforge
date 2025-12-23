#ifndef VIEWMANAGER_H
#define VIEWMANAGER_H

#include <QObject>

class QLabel;
class QScrollArea;
class QScrollBar;
class QImage;

/**
 * @class ViewManager
 * @brief Manages image display, zoom, and view transformations
 *
 * This class extracts all view-related logic from MainWindow, providing
 * a clean interface for managing how images are displayed to the user.
 *
 * Responsibilities:
 * - Display images in the view
 * - Handle zoom operations (in, out, fit to window, normal size)
 * - Manage scroll position during zoom
 * - Track current scale factor
 *
 * Benefits:
 * - Separates view logic from business logic
 * - Makes zoom/display code reusable
 * - Simplifies MainWindow
 * - Single Responsibility Principle compliance
 */
class ViewManager : public QObject
{
    Q_OBJECT

public:
    explicit ViewManager(QLabel *imageLabel, QScrollArea *scrollArea, QObject *parent = nullptr);

    // Display an image
    void displayImage(const QImage &image);

    // Zoom operations
    void zoomIn();
    void zoomOut();
    void normalSize();
    void setFitToWindow(bool fit);
    bool isFitToWindow() const { return m_fitToWindow; }

    // Scale factor
    double scaleFactor() const { return m_scaleFactor; }
    void setScaleFactor(double factor);

    // Check if zoom operations are possible
    bool canZoomIn() const { return m_scaleFactor < 3.0; }
    bool canZoomOut() const { return m_scaleFactor > 0.333; }

    // Reset view to initial state
    void reset();

signals:
    // Emitted when scale factor changes
    void scaleFactorChanged(double factor);

    // Emitted when fit-to-window mode changes
    void fitToWindowChanged(bool fit);

    // Emitted when zoom limits are reached
    void zoomLimitsChanged(bool canZoomIn, bool canZoomOut);

private:
    void scaleImage(double factor);
    void adjustScrollBar(QScrollBar *scrollBar, double factor);
    void updateZoomLimits();

    QLabel *m_imageLabel;
    QScrollArea *m_scrollArea;
    double m_scaleFactor;
    bool m_fitToWindow;
};

#endif // VIEWMANAGER_H
