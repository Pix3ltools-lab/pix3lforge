#include "commandmanager.h"
#include "../model/imagedocument.h"
#include "../imageprocessor.h"
#include "../logging/logger.h"
#include <QUndoCommand>

CommandManager::CommandManager(ImageDocument *document, ImageProcessor *processor, QObject *parent)
    : QObject(parent)
    , m_document(document)
    , m_processor(processor)
    , m_undoStack(new QUndoStack(this))
{
    // Connect undo stack signals to our signals
    connect(m_undoStack, &QUndoStack::canUndoChanged, this, &CommandManager::canUndoChanged);
    connect(m_undoStack, &QUndoStack::canRedoChanged, this, &CommandManager::canRedoChanged);
    connect(m_undoStack, &QUndoStack::indexChanged, this, &CommandManager::indexChanged);
}

CommandManager::~CommandManager()
{
    // QUndoStack is deleted automatically as a child of this object
}

void CommandManager::executeCommand(QUndoCommand *command)
{
    if (command) {
        LOG_DEBUG(QString("Executing command: %1").arg(command->text()));
        m_undoStack->push(command);
        emit commandExecuted();
    }
}

void CommandManager::clear()
{
    int count = m_undoStack->count();
    LOG_DEBUG(QString("Clearing undo stack (%1 commands)").arg(count));
    m_undoStack->clear();
}

bool CommandManager::canUndo() const
{
    return m_undoStack->canUndo();
}

bool CommandManager::canRedo() const
{
    return m_undoStack->canRedo();
}

QAction* CommandManager::createUndoAction(QObject *parent, const QString &prefix) const
{
    return m_undoStack->createUndoAction(parent, prefix);
}

QAction* CommandManager::createRedoAction(QObject *parent, const QString &prefix) const
{
    return m_undoStack->createRedoAction(parent, prefix);
}
