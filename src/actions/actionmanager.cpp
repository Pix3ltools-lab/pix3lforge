#include "actionmanager.h"
#include "../mainwindow.h"
#include "../commands/commandmanager.h"
#include "../settings/settingsmanager.h"
#include <QMainWindow>
#include <QAction>
#include <QApplication>
#include <QMenu>
#include <QFileInfo>
#include <QDir>

ActionManager::ActionManager(QMainWindow *mainWindow, CommandManager *commandManager, QObject *parent)
    : QObject(parent)
    , m_mainWindow(mainWindow)
    , m_commandManager(commandManager)
    , m_recentFilesMenu(nullptr)
    , m_saveAct(nullptr)
    , m_saveAsAct(nullptr)
    , m_undoAct(nullptr)
    , m_redoAct(nullptr)
    , m_zoomInAct(nullptr)
    , m_zoomOutAct(nullptr)
    , m_normalSizeAct(nullptr)
    , m_fitToWindowAct(nullptr)
{
}

void ActionManager::createAllActions()
{
    createFileActions();
    createEditActions();
    createViewActions();
    createFilterActions();
    createTransformActions();
    createWatermarkActions();
    createAIActions();
    createHelpActions();
}

void ActionManager::createFileActions()
{
    MainWindow *mainWin = qobject_cast<MainWindow*>(m_mainWindow);
    if (!mainWin) return;

    QAction *openAct = new QAction(tr("&Open..."), m_mainWindow);
    openAct->setShortcut(QKeySequence::Open);
    openAct->setIcon(QIcon(":/icons/icons/toolbar/open.svg"));
    connect(openAct, &QAction::triggered, mainWin, &MainWindow::open);

    // Create recent files menu
    m_recentFilesMenu = new QMenu(tr("Open &Recent"), m_mainWindow);
    updateRecentFilesMenu();

    // Connect to SettingsManager to update menu when recent files change
    connect(SettingsManager::instance(), &SettingsManager::recentFilesChanged,
            this, &ActionManager::updateRecentFilesMenu);

    m_saveAct = new QAction(tr("&Save"), m_mainWindow);
    m_saveAct->setShortcut(QKeySequence::Save);
    m_saveAct->setIcon(QIcon(":/icons/icons/toolbar/save.svg"));
    m_saveAct->setEnabled(false);
    connect(m_saveAct, &QAction::triggered, mainWin, &MainWindow::save);

    m_saveAsAct = new QAction(tr("Save &As..."), m_mainWindow);
    m_saveAsAct->setShortcut(QKeySequence::SaveAs);
    m_saveAsAct->setIcon(QIcon(":/icons/icons/toolbar/save.svg"));
    m_saveAsAct->setEnabled(false);
    connect(m_saveAsAct, &QAction::triggered, mainWin, &MainWindow::saveAs);

    QAction *exitAct = new QAction(tr("E&xit"), m_mainWindow);
    exitAct->setShortcut(tr("Ctrl+Q"));
    connect(exitAct, &QAction::triggered, m_mainWindow, &QMainWindow::close);

    m_fileActions << openAct << m_saveAct << m_saveAsAct << exitAct;
}

void ActionManager::updateRecentFilesMenu()
{
    if (!m_recentFilesMenu) return;

    MainWindow *mainWin = qobject_cast<MainWindow*>(m_mainWindow);
    if (!mainWin) return;

    // Clear existing actions
    m_recentFilesMenu->clear();

    QStringList recentFiles = SettingsManager::instance()->recentFiles();

    if (recentFiles.isEmpty()) {
        m_recentFilesMenu->setEnabled(false);
        QAction *noFilesAct = m_recentFilesMenu->addAction(tr("No recent files"));
        noFilesAct->setEnabled(false);
        return;
    }

    m_recentFilesMenu->setEnabled(true);

    // Add recent files as actions
    for (int i = 0; i < recentFiles.size(); ++i) {
        QString filePath = recentFiles[i];  // Make a copy!
        QFileInfo fileInfo(filePath);

        // Create action with shortened display name
        QString displayName = QString("&%1 %2").arg(i + 1).arg(fileInfo.fileName());
        QAction *recentFileAct = new QAction(displayName, m_recentFilesMenu);
        recentFileAct->setStatusTip(QDir::toNativeSeparators(filePath));
        recentFileAct->setData(filePath); // Store full path in data

        // Connect to load recent file - capture filePath by value
        connect(recentFileAct, &QAction::triggered, this, [mainWin, filePath]() {
            mainWin->loadFile(filePath);
        });

        m_recentFilesMenu->addAction(recentFileAct);
    }

    // Add separator and "Clear Recent Files" action
    m_recentFilesMenu->addSeparator();
    QAction *clearAct = m_recentFilesMenu->addAction(tr("Clear Recent Files"));
    connect(clearAct, &QAction::triggered, this, []() {
        SettingsManager::instance()->clearRecentFiles();
    });
}

void ActionManager::createEditActions()
{
    MainWindow *mainWin = qobject_cast<MainWindow*>(m_mainWindow);
    if (!mainWin) return;

    // Undo/Redo
    m_undoAct = m_commandManager->createUndoAction(m_mainWindow, tr("&Undo"));
    m_undoAct->setShortcut(QKeySequence::Undo);
    m_undoAct->setIcon(QIcon(":/icons/icons/toolbar/undo.svg"));

    m_redoAct = m_commandManager->createRedoAction(m_mainWindow, tr("&Redo"));
    m_redoAct->setShortcut(QKeySequence::Redo);
    m_redoAct->setIcon(QIcon(":/icons/icons/toolbar/redo.svg"));

    // Auto-enhancement
    QAction *autoEnhanceAct = new QAction(tr("Auto &Enhance"), m_mainWindow);
    autoEnhanceAct->setShortcut(tr("Ctrl+E"));
    autoEnhanceAct->setIcon(QIcon(":/icons/icons/toolbar/auto_enhance.svg"));
    connect(autoEnhanceAct, &QAction::triggered, mainWin, &MainWindow::autoEnhance);

    // Basic adjustments
    QAction *brightnessAct = new QAction(tr("&Brightness..."), m_mainWindow);
    brightnessAct->setShortcut(tr("Ctrl+B"));
    brightnessAct->setIcon(QIcon(":/icons/icons/toolbar/brightness.svg"));
    connect(brightnessAct, &QAction::triggered, mainWin, &MainWindow::adjustBrightness);

    QAction *contrastAct = new QAction(tr("&Contrast..."), m_mainWindow);
    contrastAct->setShortcut(tr("Ctrl+Shift+C"));
    contrastAct->setIcon(QIcon(":/icons/icons/toolbar/contrast.svg"));
    connect(contrastAct, &QAction::triggered, mainWin, &MainWindow::adjustContrast);

    QAction *saturationAct = new QAction(tr("&Saturation..."), m_mainWindow);
    connect(saturationAct, &QAction::triggered, mainWin, &MainWindow::adjustSaturation);

    QAction *hueAct = new QAction(tr("&Hue..."), m_mainWindow);
    connect(hueAct, &QAction::triggered, mainWin, &MainWindow::adjustHue);

    QAction *gammaAct = new QAction(tr("&Gamma..."), m_mainWindow);
    connect(gammaAct, &QAction::triggered, mainWin, &MainWindow::adjustGamma);

    // Color adjustments
    QAction *temperatureAct = new QAction(tr("Color &Temperature..."), m_mainWindow);
    connect(temperatureAct, &QAction::triggered, mainWin, &MainWindow::adjustColorTemperature);

    QAction *exposureAct = new QAction(tr("&Exposure..."), m_mainWindow);
    connect(exposureAct, &QAction::triggered, mainWin, &MainWindow::adjustExposure);

    QAction *shadowsAct = new QAction(tr("&Shadows..."), m_mainWindow);
    connect(shadowsAct, &QAction::triggered, mainWin, &MainWindow::adjustShadows);

    QAction *highlightsAct = new QAction(tr("&Highlights..."), m_mainWindow);
    connect(highlightsAct, &QAction::triggered, mainWin, &MainWindow::adjustHighlights);

    m_editActions << m_undoAct << m_redoAct << autoEnhanceAct
                  << brightnessAct << contrastAct << saturationAct << hueAct << gammaAct
                  << temperatureAct << exposureAct << shadowsAct << highlightsAct;
}

void ActionManager::createViewActions()
{
    MainWindow *mainWin = qobject_cast<MainWindow*>(m_mainWindow);
    if (!mainWin) return;

    m_zoomInAct = new QAction(tr("Zoom &In (25%)"), m_mainWindow);
    m_zoomInAct->setShortcut(QKeySequence::ZoomIn);
    m_zoomInAct->setIcon(QIcon(":/icons/icons/toolbar/zoom_in.svg"));
    m_zoomInAct->setEnabled(false);
    connect(m_zoomInAct, &QAction::triggered, mainWin, &MainWindow::zoomIn);

    m_zoomOutAct = new QAction(tr("Zoom &Out (25%)"), m_mainWindow);
    m_zoomOutAct->setShortcut(QKeySequence::ZoomOut);
    m_zoomOutAct->setIcon(QIcon(":/icons/icons/toolbar/zoom_out.svg"));
    m_zoomOutAct->setEnabled(false);
    connect(m_zoomOutAct, &QAction::triggered, mainWin, &MainWindow::zoomOut);

    m_normalSizeAct = new QAction(tr("&Normal Size"), m_mainWindow);
    m_normalSizeAct->setShortcut(tr("Ctrl+0"));
    m_normalSizeAct->setIcon(QIcon(":/icons/icons/toolbar/zoom_normal.svg"));
    m_normalSizeAct->setEnabled(false);
    connect(m_normalSizeAct, &QAction::triggered, mainWin, &MainWindow::normalSize);

    m_fitToWindowAct = new QAction(tr("&Fit to Window"), m_mainWindow);
    m_fitToWindowAct->setEnabled(false);
    m_fitToWindowAct->setCheckable(true);
    m_fitToWindowAct->setShortcut(tr("Ctrl+F"));
    m_fitToWindowAct->setIcon(QIcon(":/icons/icons/toolbar/fit_to_window.svg"));
    connect(m_fitToWindowAct, &QAction::triggered, mainWin, &MainWindow::fitToWindow);

    m_viewActions << m_zoomInAct << m_zoomOutAct << m_normalSizeAct << m_fitToWindowAct;
}

void ActionManager::createFilterActions()
{
    MainWindow *mainWin = qobject_cast<MainWindow*>(m_mainWindow);
    if (!mainWin) return;

    QAction *blackWhiteAct = new QAction(tr("&Black & White"), m_mainWindow);
    blackWhiteAct->setShortcut(tr("Ctrl+Shift+B"));
    blackWhiteAct->setIcon(QIcon(":/icons/icons/toolbar/blackwhite.svg"));
    connect(blackWhiteAct, &QAction::triggered, mainWin, &MainWindow::applyBlackAndWhite);

    QAction *sepiaAct = new QAction(tr("&Sepia"), m_mainWindow);
    connect(sepiaAct, &QAction::triggered, mainWin, &MainWindow::applySepia);

    QAction *vignetteAct = new QAction(tr("&Vignette"), m_mainWindow);
    connect(vignetteAct, &QAction::triggered, mainWin, &MainWindow::applyVignette);

    QAction *sharpenAct = new QAction(tr("S&harpen"), m_mainWindow);
    sharpenAct->setIcon(QIcon(":/icons/icons/toolbar/sharpen.svg"));
    connect(sharpenAct, &QAction::triggered, mainWin, &MainWindow::applySharpen);

    QAction *blurAct = new QAction(tr("&Blur..."), m_mainWindow);
    blurAct->setIcon(QIcon(":/icons/icons/toolbar/blur.svg"));
    connect(blurAct, &QAction::triggered, mainWin, &MainWindow::applyBlur);

    QAction *edgeDetectionAct = new QAction(tr("&Edge Detection"), m_mainWindow);
    connect(edgeDetectionAct, &QAction::triggered, mainWin, &MainWindow::applyEdgeDetection);

    m_filterActions << blackWhiteAct << sepiaAct << vignetteAct << sharpenAct << blurAct << edgeDetectionAct;
}

void ActionManager::createTransformActions()
{
    MainWindow *mainWin = qobject_cast<MainWindow*>(m_mainWindow);
    if (!mainWin) return;

    QAction *rotate90Act = new QAction(tr("Rotate &90° CW"), m_mainWindow);
    rotate90Act->setShortcut(tr("Ctrl+R"));
    rotate90Act->setIcon(QIcon(":/icons/icons/toolbar/rotate.svg"));
    connect(rotate90Act, &QAction::triggered, mainWin, &MainWindow::rotate90);

    QAction *rotate180Act = new QAction(tr("Rotate &180°"), m_mainWindow);
    connect(rotate180Act, &QAction::triggered, mainWin, &MainWindow::rotate180);

    QAction *rotate270Act = new QAction(tr("Rotate &270° CW"), m_mainWindow);
    connect(rotate270Act, &QAction::triggered, mainWin, &MainWindow::rotate270);

    QAction *flipHorizontalAct = new QAction(tr("Flip &Horizontal"), m_mainWindow);
    flipHorizontalAct->setShortcut(tr("Ctrl+H"));
    flipHorizontalAct->setIcon(QIcon(":/icons/icons/toolbar/flip_horizontal.svg"));
    connect(flipHorizontalAct, &QAction::triggered, mainWin, &MainWindow::flipHorizontal);

    QAction *flipVerticalAct = new QAction(tr("Flip &Vertical"), m_mainWindow);
    flipVerticalAct->setShortcut(tr("Ctrl+Shift+V"));
    connect(flipVerticalAct, &QAction::triggered, mainWin, &MainWindow::flipVertical);

    QAction *resizeAct = new QAction(tr("&Resize..."), m_mainWindow);
    resizeAct->setIcon(QIcon(":/icons/icons/toolbar/resize.svg"));
    connect(resizeAct, &QAction::triggered, mainWin, &MainWindow::resizeImage);

    QAction *cropAct = new QAction(tr("&Crop..."), m_mainWindow);
    cropAct->setShortcut(tr("Ctrl+Shift+X"));
    connect(cropAct, &QAction::triggered, mainWin, &MainWindow::cropImage);

    m_transformActions << rotate90Act << rotate180Act << rotate270Act
                       << flipHorizontalAct << flipVerticalAct
                       << resizeAct << cropAct;
}

void ActionManager::createWatermarkActions()
{
    MainWindow *mainWin = qobject_cast<MainWindow*>(m_mainWindow);
    if (!mainWin) return;

    QAction *textWatermarkAct = new QAction(tr("Text &Watermark..."), m_mainWindow);
    connect(textWatermarkAct, &QAction::triggered, mainWin, &MainWindow::addTextWatermark);

    QAction *imageWatermarkAct = new QAction(tr("&Image Watermark..."), m_mainWindow);
    connect(imageWatermarkAct, &QAction::triggered, mainWin, &MainWindow::addImageWatermark);

    m_watermarkActions << textWatermarkAct << imageWatermarkAct;
}

void ActionManager::createAIActions()
{
    MainWindow *mainWin = qobject_cast<MainWindow*>(m_mainWindow);
    if (!mainWin) return;

    m_aiEnhanceAct = new QAction(tr("AI &Enhance..."), m_mainWindow);
    m_aiEnhanceAct->setStatusTip(tr("Get AI-powered enhancement suggestions"));
    m_aiEnhanceAct->setIcon(QIcon(":/icons/icons/toolbar/ai_enhance.svg"));
    m_aiEnhanceAct->setEnabled(false);  // Enabled when image loaded
    connect(m_aiEnhanceAct, &QAction::triggered, mainWin, &MainWindow::aiEnhance);

    m_aiSettingsAct = new QAction(tr("AI &Settings..."), m_mainWindow);
    m_aiSettingsAct->setStatusTip(tr("Configure AI provider settings"));
    connect(m_aiSettingsAct, &QAction::triggered, mainWin, &MainWindow::showAISettings);

    m_aiActions << m_aiEnhanceAct << m_aiSettingsAct;
}

void ActionManager::createHelpActions()
{
    MainWindow *mainWin = qobject_cast<MainWindow*>(m_mainWindow);
    if (!mainWin) return;

    m_viewLogsAction = new QAction(tr("View &Logs"), m_mainWindow);
    m_viewLogsAction->setStatusTip(tr("View application logs"));
    m_viewLogsAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_L));
    connect(m_viewLogsAction, &QAction::triggered, mainWin, &MainWindow::showLogViewer);

    m_aboutAction = new QAction(tr("&About Pix3lForge"), m_mainWindow);
    connect(m_aboutAction, &QAction::triggered, mainWin, &MainWindow::about);

    QAction *aboutQtAct = new QAction(tr("About &Qt"), m_mainWindow);
    connect(aboutQtAct, &QAction::triggered, qApp, &QApplication::aboutQt);

    m_helpActions << m_viewLogsAction << m_aboutAction << aboutQtAct;
}
