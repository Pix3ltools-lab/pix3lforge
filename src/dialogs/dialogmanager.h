#ifndef DIALOGMANAGER_H
#define DIALOGMANAGER_H

#include <QObject>
#include <QString>
#include <QImage>
#include <optional>

class QWidget;

/**
 * @struct ResizeParams
 * @brief Parameters for image resize operation
 */
struct ResizeParams {
    int width;
    int height;
};

/**
 * @struct CropParams
 * @brief Parameters for image crop operation
 */
struct CropParams {
    int x;
    int y;
    int width;
    int height;
};

/**
 * @struct WatermarkParams
 * @brief Parameters for text watermark operation
 */
struct WatermarkParams {
    QString text;
    int x;
    int y;
};

/**
 * @struct ImageWatermarkParams
 * @brief Parameters for image watermark operation
 */
struct ImageWatermarkParams {
    QImage watermark;
    int x;
    int y;
};

/**
 * @class DialogManager
 * @brief Manages all user input dialogs and file operations
 *
 * This class extracts all QInputDialog, QFileDialog, and QMessageBox
 * interactions from MainWindow, providing a clean interface for
 * requesting user input.
 *
 * Responsibilities:
 * - Show input dialogs for adjustments
 * - Show file dialogs for open/save operations
 * - Show message boxes for errors/confirmations
 * - Validate user input
 * - Return structured parameters
 *
 * Benefits:
 * - Centralizes all user interaction
 * - Makes dialogs easier to test (can mock)
 * - Reduces MainWindow complexity
 * - Consistent error handling
 */
class DialogManager : public QObject
{
    Q_OBJECT

public:
    explicit DialogManager(QWidget *parent = nullptr);

    // File dialogs
    QString showOpenFileDialog();
    QString showSaveFileDialog(int *quality = nullptr);

    // Adjustment input dialogs
    std::optional<int> showBrightnessDialog();
    std::optional<int> showContrastDialog();
    std::optional<int> showSaturationDialog();
    std::optional<int> showHueDialog();
    std::optional<double> showGammaDialog();
    std::optional<int> showColorTemperatureDialog();
    std::optional<int> showExposureDialog();
    std::optional<int> showShadowsDialog();
    std::optional<int> showHighlightsDialog();

    // Filter input dialogs
    std::optional<int> showBlurRadiusDialog();

    // Transformation input dialogs
    std::optional<ResizeParams> showResizeDialog(int currentWidth, int currentHeight);
    std::optional<CropParams> showCropDialog(int imageWidth, int imageHeight);

    // Watermark input dialogs
    std::optional<WatermarkParams> showTextWatermarkDialog(int imageWidth, int imageHeight);
    std::optional<ImageWatermarkParams> showImageWatermarkDialog(int imageWidth, int imageHeight);

    // Message dialogs
    void showError(const QString &title, const QString &message);
    void showInfo(const QString &title, const QString &message);
    void showAbout();

private:
    QWidget *m_parent;
};

#endif // DIALOGMANAGER_H
