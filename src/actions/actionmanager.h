#ifndef ACTIONMANAGER_H
#define ACTIONMANAGER_H

#include <QObject>
#include <QList>

class QAction;
class QMenu;
class QMainWindow;
class CommandManager;

/**
 * @class ActionManager
 * @brief Manages all QActions for the application
 *
 * This class is responsible for creating and managing all QActions
 * used in menus and toolbars. It keeps MainWindow cleaner by
 * centralizing action creation and management.
 */
class ActionManager : public QObject
{
    Q_OBJECT

public:
    explicit ActionManager(QMainWindow *mainWindow, CommandManager *commandManager, QObject *parent = nullptr);

    // Create all actions
    void createAllActions();

    // Update recent files menu
    void updateRecentFilesMenu();

    // Action getters for menu/toolbar creation
    QList<QAction*> fileActions() const { return m_fileActions; }
    QMenu* recentFilesMenu() const { return m_recentFilesMenu; }
    QList<QAction*> editActions() const { return m_editActions; }
    QList<QAction*> filterActions() const { return m_filterActions; }
    QList<QAction*> transformActions() const { return m_transformActions; }
    QList<QAction*> watermarkActions() const { return m_watermarkActions; }
    QList<QAction*> viewActions() const { return m_viewActions; }
    QList<QAction*> aiActions() const { return m_aiActions; }
    QList<QAction*> helpActions() const { return m_helpActions; }

    // Individual action getters for enabling/disabling
    QAction* saveAction() const { return m_saveAct; }
    QAction* saveAsAction() const { return m_saveAsAct; }
    QAction* undoAction() const { return m_undoAct; }
    QAction* redoAction() const { return m_redoAct; }
    QAction* zoomInAction() const { return m_zoomInAct; }
    QAction* zoomOutAction() const { return m_zoomOutAct; }
    QAction* normalSizeAction() const { return m_normalSizeAct; }
    QAction* fitToWindowAction() const { return m_fitToWindowAct; }
    QAction* aiEnhanceAction() const { return m_aiEnhanceAct; }
    QAction* aiSettingsAction() const { return m_aiSettingsAct; }

private:
    void createFileActions();
    void createEditActions();
    void createViewActions();
    void createFilterActions();
    void createTransformActions();
    void createWatermarkActions();
    void createAIActions();
    void createHelpActions();

    QMainWindow *m_mainWindow;
    CommandManager *m_commandManager;

    // Recent files menu
    QMenu *m_recentFilesMenu;

    // Individual actions that need to be accessed
    QAction *m_saveAct;
    QAction *m_saveAsAct;
    QAction *m_undoAct;
    QAction *m_redoAct;
    QAction *m_zoomInAct;
    QAction *m_zoomOutAct;
    QAction *m_normalSizeAct;
    QAction *m_fitToWindowAct;

    // Action lists for menu/toolbar creation
    QList<QAction*> m_fileActions;
    QList<QAction*> m_editActions;
    QList<QAction*> m_filterActions;
    QList<QAction*> m_transformActions;
    QList<QAction*> m_watermarkActions;
    QList<QAction*> m_aiActions;
    QList<QAction*> m_viewActions;
    QList<QAction*> m_helpActions;

    // AI actions
    QAction* m_aiEnhanceAct;
    QAction* m_aiSettingsAct;

    // Help actions
    QAction* m_aboutAction;
    QAction* m_viewLogsAction;
};

#endif // ACTIONMANAGER_H
