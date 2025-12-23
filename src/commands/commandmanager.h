#ifndef COMMANDMANAGER_H
#define COMMANDMANAGER_H

#include <QObject>
#include <QUndoStack>

class ImageDocument;
class ImageProcessor;

/**
 * @class CommandManager
 * @brief Manages the command history and undo/redo functionality
 *
 * This class encapsulates all undo/redo logic that was previously in MainWindow.
 * It provides a clean interface for executing commands and managing the undo stack.
 *
 * Responsibilities:
 * - Manage QUndoStack lifecycle
 * - Execute commands through the stack
 * - Provide undo/redo actions for UI binding
 * - Track command execution state
 *
 * Benefits:
 * - Separates command execution from UI logic
 * - Centralizes undo/redo management
 * - Reduces MainWindow complexity
 */
class CommandManager : public QObject
{
    Q_OBJECT

public:
    explicit CommandManager(ImageDocument *document, ImageProcessor *processor, QObject *parent = nullptr);
    ~CommandManager();

    // Access to undo stack for UI integration
    QUndoStack* undoStack() { return m_undoStack; }
    const QUndoStack* undoStack() const { return m_undoStack; }

    // Execute a command
    void executeCommand(QUndoCommand *command);

    // Clear command history
    void clear();

    // Check if we can undo/redo
    bool canUndo() const;
    bool canRedo() const;

    // Get undo/redo actions for menu/toolbar
    QAction* createUndoAction(QObject *parent, const QString &prefix = QString()) const;
    QAction* createRedoAction(QObject *parent, const QString &prefix = QString()) const;

signals:
    // Emitted when command is executed
    void commandExecuted();

    // Emitted when undo/redo state changes
    void canUndoChanged(bool canUndo);
    void canRedoChanged(bool canRedo);

    // Emitted when the command stack changes
    void indexChanged(int index);

private:
    ImageDocument *m_document;
    ImageProcessor *m_processor;
    QUndoStack *m_undoStack;
};

#endif // COMMANDMANAGER_H
