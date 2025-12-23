#ifndef AIENHANCEMENTDIALOG_H
#define AIENHANCEMENTDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QProgressBar>
#include <QTextEdit>
#include "../ai/IAIProvider.h"
#include "../ai/EnhancementResponseParser.h"

/**
 * Dialog showing AI enhancement suggestions
 * User can select which suggestions to apply
 */
class AIEnhancementDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AIEnhancementDialog(const QString& imagePath, QWidget* parent = nullptr);
    ~AIEnhancementDialog();

    /**
     * Get selected enhancement suggestions
     * @return List of suggestions user wants to apply
     */
    QList<ImageEnhancementSuggestion> getSelectedSuggestions() const;

signals:
    /**
     * Emitted when user clicks "Apply Selected"
     */
    void applyEnhancements(const QList<ImageEnhancementSuggestion>& suggestions);

private slots:
    void onAnalysisCompleted(const ImageEnhancementAnalysis& analysis);
    void onAnalysisError(const QString& error);
    void onApplyClicked();
    void onCancelClicked();
    void onSelectAllClicked();
    void onDeselectAllClicked();

private:
    void setupUI();
    void startAnalysis();
    void displaySuggestions(const ImageEnhancementAnalysis& analysis);

    QString m_imagePath;
    IAIProvider* m_provider;
    ImageEnhancementAnalysis m_analysis;

    QLabel* m_statusLabel;
    QProgressBar* m_progressBar;
    QTextEdit* m_assessmentText;
    QListWidget* m_suggestionsList;
    QPushButton* m_selectAllButton;
    QPushButton* m_deselectAllButton;
    QPushButton* m_applyButton;
    QPushButton* m_cancelButton;
};

#endif // AIENHANCEMENTDIALOG_H
