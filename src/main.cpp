#include "mainwindow.h"
#include "logging/logger.h"
#include "pix3ltheme.h"
#include <QApplication>

/**
 * Custom message handler that redirects all Qt messages (qDebug, qInfo, qWarning, qCritical)
 * to the centralized Logger system (file only, no console output)
 * Responsibility: Map Qt logging to centralized Logger
 */
void customMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    // Map Qt message types to Logger levels
    Logger::LogLevel logLevel;
    switch (type) {
        case QtDebugMsg:
            logLevel = Logger::Debug;
            break;
        case QtInfoMsg:
            logLevel = Logger::Info;
            break;
        case QtWarningMsg:
            logLevel = Logger::Warning;
            break;
        case QtCriticalMsg:
        case QtFatalMsg:
            logLevel = Logger::Critical;
            break;
        default:
            logLevel = Logger::Info;
            break;
    }

    // Extract component name from context (file name without path and extension)
    QString component;
    if (context.file) {
        QString filePath(context.file);
        int lastSlash = filePath.lastIndexOf('/');
        if (lastSlash == -1) {
            lastSlash = filePath.lastIndexOf('\\');
        }
        component = filePath.mid(lastSlash + 1);

        // Remove extension
        int dotPos = component.lastIndexOf('.');
        if (dotPos != -1) {
            component = component.left(dotPos);
        }
    }

    // Log to centralized Logger (only to file, not console to prevent recursion)
    Logger& logger = Logger::instance();
    switch (logLevel) {
        case Logger::Debug:
            logger.debug(msg, component);
            break;
        case Logger::Info:
            logger.info(msg, component);
            break;
        case Logger::Warning:
            logger.warning(msg, component);
            break;
        case Logger::Error:
        case Logger::Critical:
            logger.critical(msg, component);
            break;
    }

    // For fatal errors, abort
    if (type == QtFatalMsg) {
        abort();
    }
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Set application information
    QApplication::setApplicationName("Pix3lForge");
    QApplication::setApplicationVersion("1.0.0");
    QApplication::setOrganizationName("Pix3lTools");

    // Initialize Logger BEFORE installing message handler
    Logger::instance().initialize();

    // Set log level to Debug in debug builds to capture all messages
    // For production, this should be set to Logger::Warning
    // Note: In CMake, NDEBUG is defined in Release builds, NOT defined in Debug builds
#ifndef NDEBUG
    Logger::instance().setLogLevel(Logger::Debug);
#else
    Logger::instance().setLogLevel(Logger::Warning);
#endif

    // IMPORTANT: Disable console output in Logger to prevent infinite recursion
    // The custom message handler will handle console output instead
    Logger::instance().setConsoleOutput(false);

    Logger::instance().info("=== Pix3lForge starting ===", "main");
    Logger::instance().info(QString("Version: %1").arg(QApplication::applicationVersion()), "main");

    // Install custom message handler to redirect all qDebug/qInfo/qWarning/qCritical to Logger
    // This suppresses console output - use LogViewerDialog (Help → View Logs) to view logs
    qInstallMessageHandler(customMessageHandler);

    // Apply Pix3lTools dark theme
    Pix3lTheme::applyDarkTheme(&a);

    MainWindow w;
    w.show();

    Logger::instance().info("MainWindow shown, entering event loop", "main");

    int result = a.exec();

    Logger::instance().info(QString("Application exiting with code: %1").arg(result), "main");
    return result;
}