#include "mainwindow.h"
#include "imageprocessor.h"
#include "model/imagedocument.h"
#include "widgets/propertiespanel.h"
#include "commands/imagecommand.h"
#include "commands/commandmanager.h"
#include "commands/commandfactory.h"
#include "view/viewmanager.h"
#include "dialogs/dialogmanager.h"
#include "dialogs/logviewerdialog.h"
#include "dialogs/AISettingsDialog.h"
#include "dialogs/AIEnhancementDialog.h"
#include "preview/previewmanager.h"
#include "actions/actionmanager.h"
#include "settings/settingsmanager.h"
#include "logging/logger.h"
#include "ai/IAIProvider.h"
#include "ai/EnhancementResponseParser.h"
#include <QtWidgets>
#include <QtSvg/QSvgRenderer>
#include <QTemporaryFile>

MainWindow::MainWindow()
    : imageLabel(new QLabel)
    , scrollArea(new QScrollArea)
    , document(new ImageDocument(this))
    , imageProcessor(new ImageProcessor(this))
    , commandManager(nullptr)
    , viewManager(nullptr)
    , propertiesPanel(nullptr)
    , undoView(nullptr)
    , placeholderWidget(nullptr)
    , isProcessing(false)
{
    // Enable drag and drop
    setAcceptDrops(true);

    imageLabel->setBackgroundRole(QPalette::Base);
    imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    imageLabel->setScaledContents(true);

    // Create placeholder widget for drag & drop hint
    placeholderWidget = new QWidget();
    placeholderWidget->setAttribute(Qt::WA_TransparentForMouseEvents);
    placeholderWidget->setAutoFillBackground(false);

    QVBoxLayout *placeholderLayout = new QVBoxLayout(placeholderWidget);
    placeholderLayout->setAlignment(Qt::AlignCenter);
    placeholderLayout->setContentsMargins(40, 40, 40, 40);
    placeholderLayout->setSpacing(15);

    // SVG icon label with larger size
    QLabel *iconLabel = new QLabel();
    QSvgRenderer *renderer = new QSvgRenderer(QString(":/icons/icons/image_placeholder.svg"), placeholderWidget);
    QPixmap iconPixmap(120, 120);
    iconPixmap.fill(Qt::transparent);
    QPainter painter(&iconPixmap);
    renderer->render(&painter);
    iconLabel->setPixmap(iconPixmap);
    iconLabel->setAlignment(Qt::AlignCenter);

    // Main title with larger font
    QLabel *titleLabel = new QLabel("Drag & Drop Image Here");
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(20);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setStyleSheet("color: #d0d0d0;");
    titleLabel->setAlignment(Qt::AlignCenter);

    // Subtitle
    QLabel *subtitleLabel = new QLabel("or use File → Open Image (Ctrl+O)");
    QFont subtitleFont = subtitleLabel->font();
    subtitleFont.setPointSize(12);
    subtitleLabel->setFont(subtitleFont);
    subtitleLabel->setStyleSheet("color: #a0a0a0;");
    subtitleLabel->setAlignment(Qt::AlignCenter);

    // Add widgets to layout
    placeholderLayout->addStretch(2);
    placeholderLayout->addWidget(iconLabel);
    placeholderLayout->addSpacing(20);
    placeholderLayout->addWidget(titleLabel);
    placeholderLayout->addSpacing(5);
    placeholderLayout->addWidget(subtitleLabel);
    placeholderLayout->addStretch(2);

    placeholderWidget->setVisible(true);

    scrollArea->setBackgroundRole(QPalette::Dark);
    scrollArea->setWidget(imageLabel);

    // Create main central widget with horizontal layout
    QWidget *centralWidget = new QWidget(this);
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Create a stacked widget to overlay placeholder on scroll area
    QStackedWidget *stackedWidget = new QStackedWidget();
    stackedWidget->addWidget(scrollArea);
    stackedWidget->setCurrentWidget(scrollArea);

    // Set placeholder as child of scroll area viewport for proper positioning
    placeholderWidget->setParent(scrollArea->viewport());
    placeholderWidget->raise();

    // Add stacked widget to the left (takes most space)
    mainLayout->addWidget(stackedWidget, 1);

    setCentralWidget(centralWidget);

    // Create managers
    commandManager = new CommandManager(document, imageProcessor, this);
    viewManager = new ViewManager(imageLabel, scrollArea, this);
    dialogManager = new DialogManager(this);
    previewManager = new PreviewManager(imageProcessor, this);
    actionManager = new ActionManager(this, commandManager, this);

    // Create actions through ActionManager
    actionManager->createAllActions();

    createMenus();
    createToolBars();
    createStatusBar();
    createDockWidgets(mainLayout);

    // Connect view manager signals
    connect(viewManager, &ViewManager::zoomLimitsChanged, this, [this](bool canZoomIn, bool canZoomOut) {
        actionManager->zoomInAction()->setEnabled(canZoomIn);
        actionManager->zoomOutAction()->setEnabled(canZoomOut);
    });

    // Restore window geometry from settings (or use default size if first run)
    QByteArray savedGeometry = SettingsManager::instance()->windowGeometry();
    if (!savedGeometry.isEmpty()) {
        restoreGeometry(savedGeometry);
        restoreState(SettingsManager::instance()->windowState());
    } else {
        // First run - set default size
        resize(1200, 800);
    }

    // Position placeholder widget immediately after window is set up
    QTimer::singleShot(0, this, [this]() {
        if (placeholderWidget && scrollArea) {
            QRect scrollRect = scrollArea->viewport()->rect();
            placeholderWidget->setGeometry(scrollRect);
        }
    });
}

void MainWindow::createDockWidgets(QHBoxLayout *mainLayout)
{
    // Properties Panel
    propertiesPanel = new PropertiesPanel(this);
    propertiesPanel->setFeatures(QDockWidget::NoDockWidgetFeatures);
    propertiesPanel->setTitleBarWidget(new QWidget()); // Hide title bar

    // Connect properties panel signals for live preview
    connect(propertiesPanel, &PropertiesPanel::brightnessChanged,
            this, &MainWindow::onLivePreviewBrightness);
    connect(propertiesPanel, &PropertiesPanel::contrastChanged,
            this, &MainWindow::onLivePreviewContrast);
    connect(propertiesPanel, &PropertiesPanel::saturationChanged,
            this, &MainWindow::onLivePreviewSaturation);
    connect(propertiesPanel, &PropertiesPanel::hueChanged,
            this, &MainWindow::onLivePreviewHue);
    connect(propertiesPanel, &PropertiesPanel::gammaChanged,
            this, &MainWindow::onLivePreviewGamma);
    connect(propertiesPanel, &PropertiesPanel::temperatureChanged,
            this, &MainWindow::onLivePreviewTemperature);
    connect(propertiesPanel, &PropertiesPanel::exposureChanged,
            this, &MainWindow::onLivePreviewExposure);
    connect(propertiesPanel, &PropertiesPanel::shadowsChanged,
            this, &MainWindow::onLivePreviewShadows);
    connect(propertiesPanel, &PropertiesPanel::highlightsChanged,
            this, &MainWindow::onLivePreviewHighlights);
    connect(propertiesPanel, &PropertiesPanel::applyAdjustments,
            this, &MainWindow::onApplyAdjustments);
    connect(propertiesPanel, &PropertiesPanel::resetAdjustments,
            this, &MainWindow::onResetAdjustments);

    // History Panel (Undo/Redo view)
    undoView = new QUndoView(commandManager->undoStack());
    QDockWidget *historyDock = new QDockWidget(tr("History"), this);
    historyDock->setWidget(undoView);
    historyDock->setFeatures(QDockWidget::NoDockWidgetFeatures);
    historyDock->setTitleBarWidget(new QWidget()); // Hide title bar

    // Create a splitter to make panels resizable
    QSplitter *rightSplitter = new QSplitter(Qt::Vertical, this);
    rightSplitter->addWidget(propertiesPanel);
    rightSplitter->addWidget(historyDock);

    // Set initial sizes: Properties panel gets 60%, History gets 40%
    rightSplitter->setStretchFactor(0, 60);
    rightSplitter->setStretchFactor(1, 40);

    // Set minimum and maximum width for the right panel
    rightSplitter->setMinimumWidth(300);
    rightSplitter->setMaximumWidth(400);

    mainLayout->addWidget(rightSplitter);

    // Connect command manager signals
    connect(commandManager, &CommandManager::canUndoChanged, this, [this](bool canUndo) {
        actionManager->undoAction()->setEnabled(canUndo);
    });
    connect(commandManager, &CommandManager::canRedoChanged, this, [this](bool canRedo) {
        actionManager->redoAction()->setEnabled(canRedo);
    });
    connect(commandManager, &CommandManager::indexChanged, this, &MainWindow::updateImageDisplay);
}

QImage MainWindow::applyCurrentAdjustments(const QImage &sourceImage)
{
    if (sourceImage.isNull() || !propertiesPanel)
        return sourceImage;

    QImage result = sourceImage;

    // Apply adjustments in sequence
    int brightness = propertiesPanel->getBrightness();
    if (brightness != 0)
        result = imageProcessor->adjustBrightness(result, brightness);

    int contrast = propertiesPanel->getContrast();
    if (contrast != 0)
        result = imageProcessor->adjustContrast(result, contrast);

    int saturation = propertiesPanel->getSaturation();
    if (saturation != 0)
        result = imageProcessor->adjustSaturation(result, saturation);

    int hue = propertiesPanel->getHue();
    if (hue != 0)
        result = imageProcessor->adjustHue(result, hue);

    double gamma = propertiesPanel->getGamma();
    if (qAbs(gamma - 1.0) > 0.01)
        result = imageProcessor->adjustGamma(result, gamma);

    int temperature = propertiesPanel->getColorTemperature();
    if (temperature != 0)
        result = imageProcessor->adjustColorTemperature(result, temperature);

    int exposure = propertiesPanel->getExposure();
    if (exposure != 0)
        result = imageProcessor->adjustExposure(result, exposure);

    int shadows = propertiesPanel->getShadows();
    if (shadows != 0)
        result = imageProcessor->adjustShadows(result, shadows);

    int highlights = propertiesPanel->getHighlights();
    if (highlights != 0)
        result = imageProcessor->adjustHighlights(result, highlights);

    return result;
}

// Get preview-sized version for faster processing
QImage MainWindow::getPreviewImage(const QImage &sourceImage)
{
    return previewManager->getOptimizedPreviewSource(sourceImage);
}

// Live preview slots
void MainWindow::onLivePreviewBrightness(int value)
{
    if (!document->isEmpty() && !isProcessing) {
        isProcessing = true;

        // Use current image as base (so preview works after undo/redo)
        QImage previewSource = getPreviewImage(document->getCurrentImage());
        QImage adjustedPreview = applyCurrentAdjustments(previewSource);

        // Scale back to display size if needed
        if (previewSource.size() != document->getCurrentImage().size()) {
            adjustedPreview = adjustedPreview.scaled(document->getCurrentImage().size(),
                                                     Qt::KeepAspectRatio,
                                                     Qt::SmoothTransformation);
        }

        previewImage = adjustedPreview;
        imageLabel->setPixmap(QPixmap::fromImage(previewImage));

        isProcessing = false;
    }
}

void MainWindow::onLivePreviewContrast(int value)
{
    onLivePreviewBrightness(0); // Reuse the same logic
}

void MainWindow::onLivePreviewSaturation(int value)
{
    onLivePreviewBrightness(0);
}

void MainWindow::onLivePreviewHue(int value)
{
    onLivePreviewBrightness(0);
}

void MainWindow::onLivePreviewGamma(double value)
{
    onLivePreviewBrightness(0);
}

void MainWindow::onLivePreviewTemperature(int value)
{
    onLivePreviewBrightness(0);
}

void MainWindow::onLivePreviewExposure(int value)
{
    onLivePreviewBrightness(0);
}

void MainWindow::onLivePreviewShadows(int value)
{
    onLivePreviewBrightness(0);
}

void MainWindow::onLivePreviewHighlights(int value)
{
    onLivePreviewBrightness(0);
}

void MainWindow::onApplyAdjustments()
{
    if (document->isEmpty() || !propertiesPanel)
        return;

    // Get all adjustments from properties panel
    AdjustmentParameters params = propertiesPanel->getAdjustments();

    // Create compound command using factory
    CompoundAdjustmentCommand *compoundCmd = CommandFactory::createCompoundAdjustmentCommand(
        document->currentImagePtr(), params, tr("Apply Adjustments"));

    // Execute command through command manager
    commandManager->executeCommand(compoundCmd);

    // Reset sliders after applying (image is already updated by command)
    propertiesPanel->resetAll();

    statusBar()->showMessage(tr("Adjustments applied"), 2000);
}

void MainWindow::onResetAdjustments()
{
    if (!document->getCurrentImage().isNull()) {
        imageLabel->setPixmap(QPixmap::fromImage(document->getCurrentImage()));
        previewImage = document->getCurrentImage();
    }
}

void MainWindow::updateImageDisplay()
{
    if (!document->isEmpty()) {
        viewManager->displayImage(document->getCurrentImage());
        previewImage = document->getCurrentImage();
        updateActions();

        // Reset properties panel after undo/redo to avoid confusion
        if (propertiesPanel) {
            propertiesPanel->resetAll();
        }
    }
}

bool MainWindow::loadFile(const QString &fileName)
{
    LOG_INFO(QString("User opening file: %1").arg(fileName));

    if (!document->load(fileName)) {
        // ImageDocument ha già loggato l'errore, non duplicare
        QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
                                 tr("Cannot load %1")
                                 .arg(QDir::toNativeSeparators(fileName)));
        return false;
    }

    previewImage = document->getCurrentImage();
    previewSourceImage = getPreviewImage(document->getCurrentImage()); // Precalculate for speed
    viewManager->displayImage(document->getCurrentImage());
    viewManager->reset();

    // Hide placeholder when image is loaded
    if (placeholderWidget)
        placeholderWidget->setVisible(false);

    updateActions();

    // Reset properties panel
    if (propertiesPanel)
        propertiesPanel->resetAll();

    // Clear undo stack for new image
    commandManager->clear();

    // Add to recent files
    SettingsManager::instance()->addRecentFile(fileName);

    LOG_INFO(QString("File opened successfully, size: %1x%2").arg(document->width()).arg(document->height()));

    QString message = tr("Opened \"%1\", %2x%3, Depth: %4")
        .arg(QDir::toNativeSeparators(fileName))
        .arg(document->width())
        .arg(document->height())
        .arg(document->depth());
    statusBar()->showMessage(message);
    return true;
}

void MainWindow::open()
{
    QString fileName = dialogManager->showOpenFileDialog();
    if (!fileName.isEmpty()) {
        loadFile(fileName);
    }
}

void MainWindow::save()
{
    if (document->filePath().isEmpty()) {
        saveAs();
    } else {
        if (!document->save()) {
            QMessageBox::warning(this, tr("Save Error"),
                                 tr("Cannot save image to %1").arg(document->filePath()));
        } else {
            statusBar()->showMessage(tr("Saved successfully"), 2000);
        }
    }
}

void MainWindow::saveAs()
{
    int quality = -1;
    QString fileName = dialogManager->showSaveFileDialog(&quality);

    if (!fileName.isEmpty()) {
        if (!document->saveAs(fileName)) {
            dialogManager->showError(tr("Save Error"),
                                     tr("Cannot save image to %1").arg(fileName));
        } else {
            statusBar()->showMessage(tr("Saved as %1").arg(fileName), 2000);
        }
    }
}

void MainWindow::about()
{
    dialogManager->showAbout();
}

void MainWindow::showLogViewer()
{
    LogViewerDialog dialog(this);
    dialog.exec();
}

// Auto-enhancement
void MainWindow::autoEnhance()
{
    if (document->isEmpty()) return;

    LOG_INFO("Auto-enhance started");

    // Analyze the current image
    ImageStats stats = imageProcessor->analyzeImage(document->getCurrentImage());

    // Get suggested enhancements
    AdjustmentParameters params = imageProcessor->suggestEnhancements(stats);

    // Check if any adjustments are needed
    if (!params.hasAnyAdjustments()) {
        LOG_INFO("Auto-enhance: no adjustments needed");
        statusBar()->showMessage(tr("Image is already well-balanced, no adjustments needed"), 3000);
        return;
    }

    LOG_INFO(QString("Auto-enhance applied: brightness=%1, contrast=%2, saturation=%3, shadows=%4, highlights=%5")
             .arg(params.brightness).arg(params.contrast).arg(params.saturation)
             .arg(params.shadows).arg(params.highlights));

    // Create compound command using factory
    CompoundAdjustmentCommand *cmd = CommandFactory::createCompoundAdjustmentCommand(
        document->currentImagePtr(), params, tr("Auto Enhance"));

    // Execute command through command manager
    commandManager->executeCommand(cmd);

    // Update the PropertiesPanel sliders to show the applied adjustments
    if (propertiesPanel) {
        propertiesPanel->setAdjustments(params);
    }

    // Show status message with adjustments made
    QString message = tr("Auto Enhanced:");
    if (params.brightness != 0) message += tr(" Brightness %1").arg(params.brightness > 0 ? "+" + QString::number(params.brightness) : QString::number(params.brightness));
    if (params.contrast != 0) message += tr(" Contrast %1").arg(params.contrast > 0 ? "+" + QString::number(params.contrast) : QString::number(params.contrast));
    if (params.saturation != 0) message += tr(" Saturation %1").arg(params.saturation > 0 ? "+" + QString::number(params.saturation) : QString::number(params.saturation));
    if (params.shadows != 0) message += tr(" Shadows %1").arg(params.shadows > 0 ? "+" + QString::number(params.shadows) : QString::number(params.shadows));
    if (params.highlights != 0) message += tr(" Highlights %1").arg(params.highlights > 0 ? "+" + QString::number(params.highlights) : QString::number(params.highlights));

    statusBar()->showMessage(message, 4000);
}

// Basic adjustments using undo commands
void MainWindow::adjustBrightness()
{
    if (document->isEmpty()) return;

    auto value = dialogManager->showBrightnessDialog();
    if (value) {
        BrightnessCommand *cmd = CommandFactory::createBrightnessCommand(document->currentImagePtr(), *value);
        commandManager->executeCommand(cmd);
        statusBar()->showMessage(tr("Brightness adjusted by %1").arg(*value), 2000);
    }
}

void MainWindow::adjustContrast()
{
    if (document->isEmpty()) return;

    auto value = dialogManager->showContrastDialog();
    if (value) {
        ContrastCommand *cmd = CommandFactory::createContrastCommand(document->currentImagePtr(), *value);
        commandManager->executeCommand(cmd);
        statusBar()->showMessage(tr("Contrast adjusted by %1").arg(*value), 2000);
    }
}

void MainWindow::adjustSaturation()
{
    if (document->isEmpty()) return;

    auto value = dialogManager->showSaturationDialog();
    if (value) {
        SaturationCommand *cmd = CommandFactory::createSaturationCommand(document->currentImagePtr(), *value);
        commandManager->executeCommand(cmd);
        statusBar()->showMessage(tr("Saturation adjusted by %1").arg(*value), 2000);
    }
}

void MainWindow::adjustHue()
{
    if (document->isEmpty()) return;

    auto value = dialogManager->showHueDialog();
    if (value) {
        HueCommand *cmd = CommandFactory::createHueCommand(document->currentImagePtr(), *value);
        commandManager->executeCommand(cmd);
        statusBar()->showMessage(tr("Hue adjusted by %1").arg(*value), 2000);
    }
}

void MainWindow::adjustGamma()
{
    if (document->isEmpty()) return;

    auto value = dialogManager->showGammaDialog();
    if (value) {
        GammaCommand *cmd = CommandFactory::createGammaCommand(document->currentImagePtr(), *value);
        commandManager->executeCommand(cmd);
        statusBar()->showMessage(tr("Gamma adjusted to %1").arg(*value), 2000);
    }
}

void MainWindow::adjustColorTemperature()
{
    if (document->isEmpty()) return;

    auto value = dialogManager->showColorTemperatureDialog();
    if (value) {
        ColorTemperatureCommand *cmd = CommandFactory::createColorTemperatureCommand(document->currentImagePtr(), *value);
        commandManager->executeCommand(cmd);
        statusBar()->showMessage(tr("Color temperature adjusted by %1").arg(*value), 2000);
    }
}

void MainWindow::adjustExposure()
{
    if (document->isEmpty()) return;

    auto value = dialogManager->showExposureDialog();
    if (value) {
        ExposureCommand *cmd = CommandFactory::createExposureCommand(document->currentImagePtr(), *value);
        commandManager->executeCommand(cmd);
        statusBar()->showMessage(tr("Exposure adjusted by %1").arg(*value), 2000);
    }
}

void MainWindow::adjustShadows()
{
    if (document->isEmpty()) return;

    auto value = dialogManager->showShadowsDialog();
    if (value) {
        ShadowsCommand *cmd = CommandFactory::createShadowsCommand(document->currentImagePtr(), *value);
        commandManager->executeCommand(cmd);
        statusBar()->showMessage(tr("Shadows adjusted by %1").arg(*value), 2000);
    }
}

void MainWindow::adjustHighlights()
{
    if (document->isEmpty()) return;

    auto value = dialogManager->showHighlightsDialog();
    if (value) {
        HighlightsCommand *cmd = CommandFactory::createHighlightsCommand(document->currentImagePtr(), *value);
        commandManager->executeCommand(cmd);
        statusBar()->showMessage(tr("Highlights adjusted by %1").arg(*value), 2000);
    }
}

// Filters using undo commands
void MainWindow::applyBlackAndWhite()
{
    if (!document->isEmpty()) {
        FilterCommand *cmd = new FilterCommand(document->currentImagePtr(), FilterCommand::BlackAndWhite);
        commandManager->executeCommand(cmd);
        statusBar()->showMessage(tr("Applied Black & White filter"), 2000);
    }
}

void MainWindow::applySepia()
{
    if (!document->isEmpty()) {
        FilterCommand *cmd = new FilterCommand(document->currentImagePtr(), FilterCommand::Sepia);
        commandManager->executeCommand(cmd);
        statusBar()->showMessage(tr("Applied Sepia filter"), 2000);
    }
}

void MainWindow::applyVignette()
{
    if (!document->isEmpty()) {
        FilterCommand *cmd = new FilterCommand(document->currentImagePtr(), FilterCommand::Vignette);
        commandManager->executeCommand(cmd);
        statusBar()->showMessage(tr("Applied Vignette effect"), 2000);
    }
}

void MainWindow::applySharpen()
{
    if (!document->isEmpty()) {
        FilterCommand *cmd = new FilterCommand(document->currentImagePtr(), FilterCommand::Sharpen);
        commandManager->executeCommand(cmd);
        statusBar()->showMessage(tr("Applied Sharpen filter"), 2000);
    }
}

void MainWindow::applyBlur()
{
    if (document->isEmpty()) return;

    auto radius = dialogManager->showBlurRadiusDialog();
    if (radius) {
        BlurCommand *cmd = CommandFactory::createBlurCommand(document->currentImagePtr(), *radius);
        commandManager->executeCommand(cmd);
        statusBar()->showMessage(tr("Applied Blur filter with radius %1").arg(*radius), 2000);
    }
}

void MainWindow::applyEdgeDetection()
{
    if (!document->isEmpty()) {
        FilterCommand *cmd = new FilterCommand(document->currentImagePtr(), FilterCommand::EdgeDetection);
        commandManager->executeCommand(cmd);
        statusBar()->showMessage(tr("Applied Edge Detection filter"), 2000);
    }
}

// Transformations using undo commands
void MainWindow::rotate90()
{
    if (!document->isEmpty()) {
        RotateCommand *cmd = new RotateCommand(document->currentImagePtr(), 90);
        commandManager->executeCommand(cmd);
        statusBar()->showMessage(tr("Rotated 90° clockwise"), 2000);
    }
}

void MainWindow::rotate180()
{
    if (!document->isEmpty()) {
        RotateCommand *cmd = new RotateCommand(document->currentImagePtr(), 180);
        commandManager->executeCommand(cmd);
        statusBar()->showMessage(tr("Rotated 180°"), 2000);
    }
}

void MainWindow::rotate270()
{
    if (!document->isEmpty()) {
        RotateCommand *cmd = new RotateCommand(document->currentImagePtr(), 270);
        commandManager->executeCommand(cmd);
        statusBar()->showMessage(tr("Rotated 270° clockwise"), 2000);
    }
}

void MainWindow::flipHorizontal()
{
    if (!document->isEmpty()) {
        FlipCommand *cmd = new FlipCommand(document->currentImagePtr(), FlipCommand::Horizontal);
        commandManager->executeCommand(cmd);
        statusBar()->showMessage(tr("Flipped horizontally"), 2000);
    }
}

void MainWindow::flipVertical()
{
    if (!document->isEmpty()) {
        FlipCommand *cmd = new FlipCommand(document->currentImagePtr(), FlipCommand::Vertical);
        commandManager->executeCommand(cmd);
        statusBar()->showMessage(tr("Flipped vertically"), 2000);
    }
}

void MainWindow::resizeImage()
{
    if (document->isEmpty()) return;

    auto params = dialogManager->showResizeDialog(document->width(), document->height());
    if (params) {
        ResizeCommand *cmd = CommandFactory::createResizeCommand(document->currentImagePtr(),
                                                                  params->width, params->height);
        commandManager->executeCommand(cmd);
        statusBar()->showMessage(tr("Resized to %1x%2").arg(params->width).arg(params->height), 2000);
    }
}

void MainWindow::cropImage()
{
    if (document->isEmpty()) return;

    auto params = dialogManager->showCropDialog(document->width(), document->height());
    if (params) {
        CropCommand *cmd = CommandFactory::createCropCommand(document->currentImagePtr(),
                                                              params->x, params->y,
                                                              params->width, params->height);
        commandManager->executeCommand(cmd);
        statusBar()->showMessage(tr("Cropped to %1x%2 at (%3,%4)")
                                 .arg(params->width).arg(params->height)
                                 .arg(params->x).arg(params->y), 2000);
    }
}

// Watermarking using undo commands
void MainWindow::addTextWatermark()
{
    if (document->isEmpty()) return;

    auto params = dialogManager->showTextWatermarkDialog(document->width(), document->height());
    if (params) {
        TextWatermarkCommand *cmd = CommandFactory::createTextWatermarkCommand(document->currentImagePtr(),
                                                                                params->text,
                                                                                params->x, params->y);
        commandManager->executeCommand(cmd);
        statusBar()->showMessage(tr("Added text watermark"), 2000);
    }
}

void MainWindow::addImageWatermark()
{
    if (document->isEmpty()) return;

    auto params = dialogManager->showImageWatermarkDialog(document->width(), document->height());
    if (params) {
        ImageWatermarkCommand *cmd = CommandFactory::createImageWatermarkCommand(document->currentImagePtr(),
                                                                                  params->watermark,
                                                                                  params->x, params->y);
        commandManager->executeCommand(cmd);
        statusBar()->showMessage(tr("Added image watermark"), 2000);
    }
}

// AI Enhancement
void MainWindow::aiEnhance()
{
    if (document->isEmpty()) {
        QMessageBox::information(this, tr("No Image"),
                                 tr("Please load an image first."));
        return;
    }

    LOG_INFO("AI Enhancement: Starting analysis");

    // Validate AI provider configuration
    AIProviderConfig config = SettingsManager::instance()->getAIProviderConfig();
    if (config.endpoint.isEmpty() || config.modelName.isEmpty()) {
        LOG_WARNING("AI Enhancement: Invalid configuration (endpoint or model empty)");
        QMessageBox::warning(this, tr("AI Configuration Required"),
                             tr("Please configure your AI provider settings first.\n\n"
                                "Go to AI → AI Settings to set up your preferred provider."));
        showAISettings();
        return;
    }

    // Check if API key is required and present
    if (config.type != IAIProvider::ProviderType::LMStudio && config.apiKey.isEmpty()) {
        LOG_WARNING(QString("AI Enhancement: API key required for provider type %1")
                    .arg(static_cast<int>(config.type)));
        QMessageBox::warning(this, tr("API Key Required"),
                             tr("Your selected AI provider requires an API key.\n\n"
                                "Go to AI → AI Settings to enter your API key."));
        showAISettings();
        return;
    }

    // If the current image has been modified, save it to a temporary file
    QString imagePath;
    QTemporaryFile* tempFile = nullptr;

    if (document->isModified()) {
        LOG_INFO("AI Enhancement: Saving modified image to temp file");
        tempFile = new QTemporaryFile(QDir::tempPath() + "/pix3lforge_ai_XXXXXX.png");
        if (!tempFile->open()) {
            LOG_ERROR("AI Enhancement: Failed to create temp file");
            QMessageBox::critical(this, tr("Error"),
                                  tr("Failed to create temporary file for AI analysis."));
            delete tempFile;
            return;
        }
        imagePath = tempFile->fileName();
        if (!document->getCurrentImage().save(imagePath, "PNG")) {
            LOG_ERROR(QString("AI Enhancement: Failed to save temp file: %1").arg(imagePath));
            QMessageBox::critical(this, tr("Error"),
                                  tr("Failed to save temporary file for AI analysis."));
            delete tempFile;
            return;
        }
        LOG_INFO(QString("AI Enhancement: Temp file created: %1").arg(imagePath));
    } else {
        // Use the original file path
        imagePath = document->filePath();
        LOG_INFO(QString("AI Enhancement: Using original file: %1").arg(imagePath));
    }

    // Set waiting cursor while AI analyzes the image
    QApplication::setOverrideCursor(Qt::WaitCursor);
    LOG_INFO("AI Enhancement: Wait cursor set");

    // Show the AI enhancement dialog
    AIEnhancementDialog *dialog = new AIEnhancementDialog(imagePath, this);

    // Restore normal cursor after dialog is shown
    QApplication::restoreOverrideCursor();
    LOG_INFO("AI Enhancement: Normal cursor restored");

    // Connect the apply signal
    connect(dialog, &AIEnhancementDialog::applyEnhancements,
            this, &MainWindow::applyAIEnhancements);

    // Show dialog and cleanup temp file when done
    int result = dialog->exec();

    if (tempFile) {
        tempFile->remove();
        delete tempFile;
        LOG_INFO("AI Enhancement: Temp file cleaned up");
    }

    if (result == QDialog::Accepted) {
        LOG_INFO("AI Enhancement: User accepted suggestions");
    } else {
        LOG_INFO("AI Enhancement: User cancelled");
    }

    dialog->deleteLater();
}

void MainWindow::showAISettings()
{
    LOG_INFO("Opening AI Settings dialog");

    AISettingsDialog dialog(this);

    if (dialog.exec() == QDialog::Accepted) {
        AIProviderConfig config = dialog.getConfig();
        SettingsManager::instance()->setAIProviderConfig(config);
        LOG_INFO(QString("AI Settings saved: provider=%1, endpoint=%2, model=%3")
                 .arg(static_cast<int>(config.type))
                 .arg(config.endpoint)
                 .arg(config.modelName));
        statusBar()->showMessage(tr("AI settings saved"), 2000);
    }
}

void MainWindow::applyAIEnhancements(const QList<ImageEnhancementSuggestion>& suggestions)
{
    if (suggestions.isEmpty()) {
        LOG_WARNING("AI Enhancement: No suggestions to apply");
        return;
    }

    LOG_INFO(QString("AI Enhancement: Applying %1 suggestions").arg(suggestions.count()));

    int appliedCount = 0;

    for (const ImageEnhancementSuggestion& suggestion : suggestions) {
        QString operation = suggestion.operation.toLower();
        double value = suggestion.value;

        LOG_INFO(QString("AI Enhancement: Applying %1=%2 (confidence=%3, reason=%4)")
                 .arg(operation).arg(value)
                 .arg(suggestion.confidence).arg(suggestion.reason));

        // Map AI suggestions to appropriate commands
        if (operation == "brightness") {
            BrightnessCommand *cmd = CommandFactory::createBrightnessCommand(
                document->currentImagePtr(), static_cast<int>(value));
            commandManager->executeCommand(cmd);
            appliedCount++;
        }
        else if (operation == "contrast") {
            ContrastCommand *cmd = CommandFactory::createContrastCommand(
                document->currentImagePtr(), static_cast<int>(value));
            commandManager->executeCommand(cmd);
            appliedCount++;
        }
        else if (operation == "saturation") {
            SaturationCommand *cmd = CommandFactory::createSaturationCommand(
                document->currentImagePtr(), static_cast<int>(value));
            commandManager->executeCommand(cmd);
            appliedCount++;
        }
        else if (operation == "hue") {
            HueCommand *cmd = CommandFactory::createHueCommand(
                document->currentImagePtr(), static_cast<int>(value));
            commandManager->executeCommand(cmd);
            appliedCount++;
        }
        else if (operation == "gamma") {
            GammaCommand *cmd = CommandFactory::createGammaCommand(
                document->currentImagePtr(), value);
            commandManager->executeCommand(cmd);
            appliedCount++;
        }
        else if (operation == "temperature" || operation == "color_temperature") {
            ColorTemperatureCommand *cmd = CommandFactory::createColorTemperatureCommand(
                document->currentImagePtr(), static_cast<int>(value));
            commandManager->executeCommand(cmd);
            appliedCount++;
        }
        else if (operation == "exposure") {
            ExposureCommand *cmd = CommandFactory::createExposureCommand(
                document->currentImagePtr(), static_cast<int>(value));
            commandManager->executeCommand(cmd);
            appliedCount++;
        }
        else if (operation == "shadows") {
            ShadowsCommand *cmd = CommandFactory::createShadowsCommand(
                document->currentImagePtr(), static_cast<int>(value));
            commandManager->executeCommand(cmd);
            appliedCount++;
        }
        else if (operation == "highlights") {
            HighlightsCommand *cmd = CommandFactory::createHighlightsCommand(
                document->currentImagePtr(), static_cast<int>(value));
            commandManager->executeCommand(cmd);
            appliedCount++;
        }
        else if (operation == "sharpen") {
            FilterCommand *cmd = new FilterCommand(document->currentImagePtr(),
                                                    FilterCommand::Sharpen);
            commandManager->executeCommand(cmd);
            appliedCount++;
        }
        else if (operation == "blur") {
            // Use value as blur radius (clamp to reasonable range)
            int radius = qBound(1, static_cast<int>(value), 100);
            BlurCommand *cmd = CommandFactory::createBlurCommand(
                document->currentImagePtr(), radius);
            commandManager->executeCommand(cmd);
            appliedCount++;
        }
        else {
            LOG_WARNING(QString("AI Enhancement: Unknown operation '%1', skipping").arg(operation));
        }
    }

    LOG_INFO(QString("AI Enhancement: Applied %1 of %2 suggestions")
             .arg(appliedCount).arg(suggestions.count()));

    statusBar()->showMessage(tr("Applied %1 AI enhancement suggestions")
                             .arg(appliedCount), 3000);
}

// Zoom methods
void MainWindow::zoomIn()
{
    viewManager->zoomIn();
}

void MainWindow::zoomOut()
{
    viewManager->zoomOut();
}

void MainWindow::normalSize()
{
    viewManager->normalSize();
}

void MainWindow::fitToWindow()
{
    bool fitToWindow = actionManager->fitToWindowAction()->isChecked();
    viewManager->setFitToWindow(fitToWindow);
    updateActions();
}


void MainWindow::createMenus()
{
    QList<QAction*> fileActions = actionManager->fileActions();
    QList<QAction*> editActions = actionManager->editActions();
    QList<QAction*> filterActions = actionManager->filterActions();
    QList<QAction*> transformActions = actionManager->transformActions();
    QList<QAction*> watermarkActions = actionManager->watermarkActions();
    QList<QAction*> aiActions = actionManager->aiActions();
    QList<QAction*> viewActions = actionManager->viewActions();
    QList<QAction*> helpActions = actionManager->helpActions();

    // File menu
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(fileActions[0]); // Open
    fileMenu->addMenu(actionManager->recentFilesMenu()); // Open Recent submenu
    fileMenu->addSeparator();
    fileMenu->addAction(fileActions[1]); // Save
    fileMenu->addAction(fileActions[2]); // Save As
    fileMenu->addSeparator();
    fileMenu->addAction(fileActions[3]); // Exit

    // Edit menu
    QMenu *editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(editActions[0]); // Undo
    editMenu->addAction(editActions[1]); // Redo
    editMenu->addSeparator();
    editMenu->addAction(editActions[2]); // Auto Enhance
    editMenu->addSeparator();
    editMenu->addSection(tr("Basic Adjustments"));
    for (int i = 3; i < 8; ++i) {
        editMenu->addAction(editActions[i]);
    }
    editMenu->addSeparator();
    editMenu->addSection(tr("Color Adjustments"));
    for (int i = 8; i < editActions.size(); ++i) {
        editMenu->addAction(editActions[i]);
    }

    // Filter menu
    QMenu *filterMenu = menuBar()->addMenu(tr("&Filter"));
    filterMenu->addSection(tr("Creative Filters"));
    for (int i = 0; i < 3; ++i) {
        filterMenu->addAction(filterActions[i]);
    }
    filterMenu->addSeparator();
    filterMenu->addSection(tr("Enhancement"));
    for (int i = 3; i < filterActions.size(); ++i) {
        filterMenu->addAction(filterActions[i]);
    }

    // Transform menu
    QMenu *transformMenu = menuBar()->addMenu(tr("&Transform"));
    transformMenu->addSection(tr("Rotate"));
    for (int i = 0; i < 3; ++i) {
        transformMenu->addAction(transformActions[i]);
    }
    transformMenu->addSeparator();
    transformMenu->addSection(tr("Flip"));
    for (int i = 3; i < 5; ++i) {
        transformMenu->addAction(transformActions[i]);
    }
    transformMenu->addSeparator();
    transformMenu->addSection(tr("Resize & Crop"));
    for (int i = 5; i < transformActions.size(); ++i) {
        transformMenu->addAction(transformActions[i]);
    }

    // Watermark menu
    QMenu *watermarkMenu = menuBar()->addMenu(tr("&Watermark"));
    for (QAction *action : watermarkActions) {
        watermarkMenu->addAction(action);
    }

    // AI menu
    QMenu *aiMenu = menuBar()->addMenu(tr("&AI"));
    for (QAction *action : aiActions) {
        aiMenu->addAction(action);
    }

    // View menu
    QMenu *viewMenu = menuBar()->addMenu(tr("&View"));
    for (QAction *action : viewActions) {
        viewMenu->addAction(action);
    }

    // Help menu
    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
    for (QAction *action : helpActions) {
        helpMenu->addAction(action);
    }
}

void MainWindow::createToolBars()
{
    QList<QAction*> fileActions = actionManager->fileActions();
    QList<QAction*> editActions = actionManager->editActions();
    QList<QAction*> filterActions = actionManager->filterActions();
    QList<QAction*> transformActions = actionManager->transformActions();

    // Create single unified toolbar with modern style
    QToolBar *mainToolBar = addToolBar(tr("Main Toolbar"));
    mainToolBar->setObjectName("MainToolBar");
    mainToolBar->setMovable(false);
    mainToolBar->setIconSize(QSize(32, 32));
    mainToolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

    // File group
    mainToolBar->addAction(fileActions[0]); // Open
    mainToolBar->addAction(fileActions[1]); // Save

    // Separator
    mainToolBar->addSeparator();

    // Edit group
    mainToolBar->addAction(editActions[0]); // Undo
    mainToolBar->addAction(editActions[1]); // Redo

    // Separator
    mainToolBar->addSeparator();

    // AI Enhancement
    mainToolBar->addAction(actionManager->aiEnhanceAction());

    // Separator
    mainToolBar->addSeparator();

    // Filters group
    mainToolBar->addAction(filterActions[0]); // Black & White
    mainToolBar->addAction(filterActions[3]); // Sharpen
    mainToolBar->addAction(filterActions[4]); // Blur

    // Separator
    mainToolBar->addSeparator();

    // Transform group
    mainToolBar->addAction(transformActions[0]); // Rotate 90
    mainToolBar->addAction(transformActions[3]); // Flip Horizontal
    mainToolBar->addAction(transformActions[5]); // Resize

    // Separator
    mainToolBar->addSeparator();

    // View group
    mainToolBar->addAction(actionManager->zoomInAction());
    mainToolBar->addAction(actionManager->zoomOutAction());
    mainToolBar->addAction(actionManager->normalSizeAction());
    mainToolBar->addAction(actionManager->fitToWindowAction());
}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::updateActions()
{
    bool hasImage = !document->isEmpty();
    actionManager->saveAction()->setEnabled(hasImage);
    actionManager->saveAsAction()->setEnabled(hasImage);
    actionManager->zoomInAction()->setEnabled(hasImage);
    actionManager->zoomOutAction()->setEnabled(hasImage);
    actionManager->normalSizeAction()->setEnabled(hasImage);
    actionManager->fitToWindowAction()->setEnabled(hasImage);
    actionManager->aiEnhanceAction()->setEnabled(hasImage);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    // Save window geometry and state
    SettingsManager::instance()->setWindowGeometry(saveGeometry());
    SettingsManager::instance()->setWindowState(saveState());
    event->accept();
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    // Accept drag only if it contains URLs (files)
    if (event->mimeData()->hasUrls()) {
        // Check if at least one file is an image
        const QList<QUrl> urls = event->mimeData()->urls();
        for (const QUrl &url : urls) {
            if (url.isLocalFile()) {
                QString filePath = url.toLocalFile();
                // Check if file has image extension
                QStringList imageExtensions = {"jpg", "jpeg", "png", "bmp", "gif", "tiff", "tif", "webp"};
                QString extension = QFileInfo(filePath).suffix().toLower();
                if (imageExtensions.contains(extension)) {
                    event->acceptProposedAction();
                    return;
                }
            }
        }
    }
}

void MainWindow::dropEvent(QDropEvent *event)
{
    const QList<QUrl> urls = event->mimeData()->urls();
    if (urls.isEmpty())
        return;

    // Load the first image file found
    for (const QUrl &url : urls) {
        if (url.isLocalFile()) {
            QString filePath = url.toLocalFile();
            // Check if file has image extension
            QStringList imageExtensions = {"jpg", "jpeg", "png", "bmp", "gif", "tiff", "tif", "webp"};
            QString extension = QFileInfo(filePath).suffix().toLower();
            if (imageExtensions.contains(extension)) {
                loadFile(filePath);
                event->acceptProposedAction();
                return;
            }
        }
    }
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);

    // Center the placeholder widget in the scroll area
    if (placeholderWidget && scrollArea) {
        QRect scrollRect = scrollArea->viewport()->rect();
        placeholderWidget->setGeometry(scrollRect);
    }
}
