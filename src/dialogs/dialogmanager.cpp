#include "dialogmanager.h"
#include "aboutdialog.h"
#include <QWidget>
#include <QInputDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>
#include <QGuiApplication>

DialogManager::DialogManager(QWidget *parent)
    : QObject(parent)
    , m_parent(parent)
{
}

QString DialogManager::showOpenFileDialog()
{
    QFileDialog dialog(m_parent, tr("Open File"));
    dialog.setMimeTypeFilters(QStringList()
        << "image/jpeg" << "image/png" << "image/bmp"
        << "image/gif" << "image/x-portable-bitmap"
        << "image/x-portable-graymap" << "image/x-portable-pixmap"
        << "image/x-xbitmap" << "image/x-xpixmap");

    if (dialog.exec() == QDialog::Accepted) {
        return dialog.selectedFiles().first();
    }
    return QString();
}

QString DialogManager::showSaveFileDialog(int *quality)
{
    QFileDialog dialog(m_parent, tr("Save As"));
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setDefaultSuffix("png");

    QStringList mimeTypes;
    mimeTypes << "image/png" << "image/jpeg" << "image/bmp" << "image/tiff";
    dialog.setMimeTypeFilters(mimeTypes);

    if (dialog.exec() == QDialog::Accepted) {
        QString fileName = dialog.selectedFiles().first();
        QString selectedFilter = dialog.selectedMimeTypeFilter();

        // For JPEG, ask for quality
        if (quality && selectedFilter == "image/jpeg") {
            bool ok;
            int q = QInputDialog::getInt(m_parent, tr("JPEG Quality"),
                                         tr("Enter JPEG quality (1-100):"),
                                         90, 1, 100, 1, &ok);
            if (ok) {
                *quality = q;
                return fileName;
            }
            return QString(); // User cancelled quality dialog
        }

        return fileName;
    }
    return QString();
}

std::optional<int> DialogManager::showBrightnessDialog()
{
    bool ok;
    int value = QInputDialog::getInt(m_parent, tr("Brightness"),
                                     tr("Adjust brightness (-100 to 100):"),
                                     0, -100, 100, 1, &ok);
    return ok ? std::optional<int>(value) : std::nullopt;
}

std::optional<int> DialogManager::showContrastDialog()
{
    bool ok;
    int value = QInputDialog::getInt(m_parent, tr("Contrast"),
                                     tr("Adjust contrast (-100 to 100):"),
                                     0, -100, 100, 1, &ok);
    return ok ? std::optional<int>(value) : std::nullopt;
}

std::optional<int> DialogManager::showSaturationDialog()
{
    bool ok;
    int value = QInputDialog::getInt(m_parent, tr("Saturation"),
                                     tr("Adjust saturation (-100 to 100):"),
                                     0, -100, 100, 1, &ok);
    return ok ? std::optional<int>(value) : std::nullopt;
}

std::optional<int> DialogManager::showHueDialog()
{
    bool ok;
    int value = QInputDialog::getInt(m_parent, tr("Hue"),
                                     tr("Adjust hue (-180 to 180):"),
                                     0, -180, 180, 1, &ok);
    return ok ? std::optional<int>(value) : std::nullopt;
}

std::optional<double> DialogManager::showGammaDialog()
{
    bool ok;
    double value = QInputDialog::getDouble(m_parent, tr("Gamma"),
                                           tr("Adjust gamma (0.1 to 10.0):"),
                                           1.0, 0.1, 10.0, 1, &ok);
    return ok ? std::optional<double>(value) : std::nullopt;
}

std::optional<int> DialogManager::showColorTemperatureDialog()
{
    bool ok;
    int value = QInputDialog::getInt(m_parent, tr("Color Temperature"),
                                     tr("Adjust color temperature (-100 to 100):"),
                                     0, -100, 100, 1, &ok);
    return ok ? std::optional<int>(value) : std::nullopt;
}

std::optional<int> DialogManager::showExposureDialog()
{
    bool ok;
    int value = QInputDialog::getInt(m_parent, tr("Exposure"),
                                     tr("Adjust exposure (-100 to 100):"),
                                     0, -100, 100, 1, &ok);
    return ok ? std::optional<int>(value) : std::nullopt;
}

std::optional<int> DialogManager::showShadowsDialog()
{
    bool ok;
    int value = QInputDialog::getInt(m_parent, tr("Shadows"),
                                     tr("Adjust shadows (-100 to 100):"),
                                     0, -100, 100, 1, &ok);
    return ok ? std::optional<int>(value) : std::nullopt;
}

std::optional<int> DialogManager::showHighlightsDialog()
{
    bool ok;
    int value = QInputDialog::getInt(m_parent, tr("Highlights"),
                                     tr("Adjust highlights (-100 to 100):"),
                                     0, -100, 100, 1, &ok);
    return ok ? std::optional<int>(value) : std::nullopt;
}

std::optional<int> DialogManager::showBlurRadiusDialog()
{
    bool ok;
    int radius = QInputDialog::getInt(m_parent, tr("Blur"),
                                      tr("Set blur radius (1 to 10):"),
                                      2, 1, 10, 1, &ok);
    return ok ? std::optional<int>(radius) : std::nullopt;
}

std::optional<ResizeParams> DialogManager::showResizeDialog(int currentWidth, int currentHeight)
{
    bool ok;
    int width = QInputDialog::getInt(m_parent, tr("Resize"),
                                     tr("Enter new width:"),
                                     currentWidth, 1, 10000, 1, &ok);
    if (!ok) return std::nullopt;

    int height = QInputDialog::getInt(m_parent, tr("Resize"),
                                      tr("Enter new height:"),
                                      currentHeight, 1, 10000, 1, &ok);
    if (!ok) return std::nullopt;

    return ResizeParams{width, height};
}

std::optional<CropParams> DialogManager::showCropDialog(int imageWidth, int imageHeight)
{
    bool ok;
    int x = QInputDialog::getInt(m_parent, tr("Crop"),
                                 tr("Enter X coordinate:"),
                                 0, 0, imageWidth-1, 1, &ok);
    if (!ok) return std::nullopt;

    int y = QInputDialog::getInt(m_parent, tr("Crop"),
                                 tr("Enter Y coordinate:"),
                                 0, 0, imageHeight-1, 1, &ok);
    if (!ok) return std::nullopt;

    int width = QInputDialog::getInt(m_parent, tr("Crop"),
                                     tr("Enter width:"),
                                     imageWidth/2, 1, imageWidth-x, 1, &ok);
    if (!ok) return std::nullopt;

    int height = QInputDialog::getInt(m_parent, tr("Crop"),
                                      tr("Enter height:"),
                                      imageHeight/2, 1, imageHeight-y, 1, &ok);
    if (!ok) return std::nullopt;

    return CropParams{x, y, width, height};
}

std::optional<WatermarkParams> DialogManager::showTextWatermarkDialog(int imageWidth, int imageHeight)
{
    bool ok;
    QString text = QInputDialog::getText(m_parent, tr("Text Watermark"),
                                         tr("Enter watermark text:"),
                                         QLineEdit::Normal, tr("Sample Watermark"), &ok);
    if (!ok || text.isEmpty()) return std::nullopt;

    int x = QInputDialog::getInt(m_parent, tr("Text Watermark"),
                                 tr("Enter X position:"),
                                 10, 0, imageWidth-100, 1, &ok);
    if (!ok) return std::nullopt;

    int y = QInputDialog::getInt(m_parent, tr("Text Watermark"),
                                 tr("Enter Y position:"),
                                 imageHeight-10, 0, imageHeight-10, 1, &ok);
    if (!ok) return std::nullopt;

    return WatermarkParams{text, x, y};
}

std::optional<ImageWatermarkParams> DialogManager::showImageWatermarkDialog(int imageWidth, int imageHeight)
{
    QString fileName = QFileDialog::getOpenFileName(m_parent, tr("Select Watermark Image"),
                                                    QDir::homePath(), tr("Images (*.png *.xpm *.jpg *.bmp)"));
    if (fileName.isEmpty()) return std::nullopt;

    QImage watermark(fileName);
    if (watermark.isNull()) {
        showError(tr("Error"), tr("Cannot load watermark image"));
        return std::nullopt;
    }

    bool ok;
    int x = QInputDialog::getInt(m_parent, tr("Image Watermark"),
                                 tr("Enter X position:"),
                                 10, 0, imageWidth-watermark.width(), 1, &ok);
    if (!ok) return std::nullopt;

    int y = QInputDialog::getInt(m_parent, tr("Image Watermark"),
                                 tr("Enter Y position:"),
                                 imageHeight-watermark.height(), 0, imageHeight-watermark.height(), 1, &ok);
    if (!ok) return std::nullopt;

    return ImageWatermarkParams{watermark, x, y};
}

void DialogManager::showError(const QString &title, const QString &message)
{
    QMessageBox::warning(m_parent, title, message);
}

void DialogManager::showInfo(const QString &title, const QString &message)
{
    QMessageBox::information(m_parent, title, message);
}

void DialogManager::showAbout()
{
    AboutDialog dialog(m_parent);
    dialog.exec();
}
