#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>
#include <QString>
#include <QList>
#include <QFutureWatcher>

class QAction;
class QLabel;
class QMenu;
class QScrollArea;
class QScrollBar;
class QUndoStack;
class QUndoView;
class QProgressBar;

class ImageProcessor;
class PropertiesPanel;
class ImageDocument;
class CommandManager;
class ViewManager;
class DialogManager;
class PreviewManager;
class ActionManager;
struct ImageEnhancementSuggestion;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

public slots:
    bool loadFile(const QString &fileName);
    void open();
    void save();
    void saveAs();
    void about();
    void showLogViewer();
    void zoomIn();
    void zoomOut();
    void normalSize();
    void fitToWindow();

    // Auto-enhancement
    void autoEnhance();

    // Basic adjustments (legacy - will be replaced by properties panel)
    void adjustBrightness();
    void adjustContrast();
    void adjustSaturation();
    void adjustHue();
    void adjustGamma();
    // Color adjustments
    void adjustColorTemperature();
    void adjustExposure();
    void adjustShadows();
    void adjustHighlights();
    // Filters
    void applyBlackAndWhite();
    void applySepia();
    void applyVignette();
    void applySharpen();
    void applyBlur();
    void applyEdgeDetection();
    // Transformations
    void rotate90();
    void rotate180();
    void rotate270();
    void flipHorizontal();
    void flipVertical();
    void resizeImage();
    void cropImage();
    // Watermarking
    void addTextWatermark();
    void addImageWatermark();
    // AI enhancement
    void aiEnhance();
    void showAISettings();

protected:
    void closeEvent(QCloseEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private slots:
    // Properties panel live preview slots
    void onLivePreviewBrightness(int value);
    void onLivePreviewContrast(int value);
    void onLivePreviewSaturation(int value);
    void onLivePreviewHue(int value);
    void onLivePreviewGamma(double value);
    void onLivePreviewTemperature(int value);
    void onLivePreviewExposure(int value);
    void onLivePreviewShadows(int value);
    void onLivePreviewHighlights(int value);
    void onApplyAdjustments();
    void onResetAdjustments();

    // Update view when image changes
    void updateImageDisplay();

private:
    void createMenus();
    void createToolBars();
    void createStatusBar();
    void createDockWidgets(class QHBoxLayout *mainLayout);
    void updateActions();

    // Apply all current adjustments from properties panel to preview
    QImage applyCurrentAdjustments(const QImage &sourceImage);

    // Get preview-sized version of image for faster processing
    QImage getPreviewImage(const QImage &sourceImage);

    // Handle async preview completion
    void onPreviewReady();

    // AI enhancement
    void applyAIEnhancements(const QList<ImageEnhancementSuggestion>& suggestions);

    ImageDocument *document;   // Document managing images and file I/O
    QImage previewImage;       // Preview with temporary adjustments
    QImage previewSourceImage; // Downscaled source for fast preview
    QLabel *imageLabel;
    QScrollArea *scrollArea;
    QWidget *placeholderWidget;  // Drag & drop hint when no image loaded

    // Async processing
    QFutureWatcher<QImage> *previewWatcher;
    QProgressBar *progressBar;
    bool isProcessing;

    CommandManager *commandManager;
    ViewManager *viewManager;
    DialogManager *dialogManager;
    PreviewManager *previewManager;
    ActionManager *actionManager;
    QUndoView *undoView;
    ImageProcessor *imageProcessor;
    PropertiesPanel *propertiesPanel;
};

#endif