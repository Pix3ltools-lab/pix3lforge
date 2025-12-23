#include "AIEnhancementDialog.h"
#include "../ai/AIProviderFactory.h"
#include "../settings/settingsmanager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QMessageBox>

AIEnhancementDialog::AIEnhancementDialog(const QString& imagePath, QWidget* parent)
    : QDialog(parent)
    , m_imagePath(imagePath)
    , m_provider(nullptr)
{
    setWindowTitle(tr("AI Enhancement Suggestions"));
    resize(700, 600);
    setupUI();
    startAnalysis();
}

AIEnhancementDialog::~AIEnhancementDialog()
{
    if (m_provider) {
        delete m_provider;
    }
}

void AIEnhancementDialog::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // Status section
    QGroupBox* statusGroup = new QGroupBox(tr("Analysis Status"));
    QVBoxLayout* statusLayout = new QVBoxLayout(statusGroup);

    m_statusLabel = new QLabel(tr("Analyzing image..."));
    statusLayout->addWidget(m_statusLabel);

    m_progressBar = new QProgressBar();
    m_progressBar->setRange(0, 0);  // Indeterminate progress
    statusLayout->addWidget(m_progressBar);

    mainLayout->addWidget(statusGroup);

    // Assessment section
    QGroupBox* assessmentGroup = new QGroupBox(tr("Overall Assessment"));
    QVBoxLayout* assessmentLayout = new QVBoxLayout(assessmentGroup);

    m_assessmentText = new QTextEdit();
    m_assessmentText->setReadOnly(true);
    m_assessmentText->setMaximumHeight(100);
    m_assessmentText->setPlaceholderText(tr("AI assessment will appear here..."));
    assessmentLayout->addWidget(m_assessmentText);

    mainLayout->addWidget(assessmentGroup);

    // Suggestions section
    QGroupBox* suggestionsGroup = new QGroupBox(tr("Enhancement Suggestions"));
    QVBoxLayout* suggestionsLayout = new QVBoxLayout(suggestionsGroup);

    m_suggestionsList = new QListWidget();
    suggestionsLayout->addWidget(m_suggestionsList);

    QHBoxLayout* selectionLayout = new QHBoxLayout();
    m_selectAllButton = new QPushButton(tr("Select All"));
    connect(m_selectAllButton, &QPushButton::clicked,
            this, &AIEnhancementDialog::onSelectAllClicked);
    selectionLayout->addWidget(m_selectAllButton);

    m_deselectAllButton = new QPushButton(tr("Deselect All"));
    connect(m_deselectAllButton, &QPushButton::clicked,
            this, &AIEnhancementDialog::onDeselectAllClicked);
    selectionLayout->addWidget(m_deselectAllButton);

    selectionLayout->addStretch();
    suggestionsLayout->addLayout(selectionLayout);

    mainLayout->addWidget(suggestionsGroup);

    // Dialog buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();

    m_applyButton = new QPushButton(tr("Apply Selected"));
    m_applyButton->setEnabled(false);
    connect(m_applyButton, &QPushButton::clicked,
            this, &AIEnhancementDialog::onApplyClicked);
    buttonLayout->addWidget(m_applyButton);

    m_cancelButton = new QPushButton(tr("Cancel"));
    connect(m_cancelButton, &QPushButton::clicked,
            this, &AIEnhancementDialog::onCancelClicked);
    buttonLayout->addWidget(m_cancelButton);

    mainLayout->addLayout(buttonLayout);

    // Disable suggestion controls initially
    m_selectAllButton->setEnabled(false);
    m_deselectAllButton->setEnabled(false);
}

void AIEnhancementDialog::startAnalysis()
{
    // Get AI provider config from settings
    AIProviderConfig config = SettingsManager::instance()->getAIProviderConfig();

    // Create provider
    m_provider = AIProviderFactory::createProvider(config, this);

    if (!m_provider) {
        m_statusLabel->setText(tr("Error: Failed to create AI provider"));
        m_statusLabel->setStyleSheet("color: red;");
        m_progressBar->setRange(0, 1);
        m_progressBar->setValue(1);
        return;
    }

    // Connect signals
    connect(m_provider, &IAIProvider::enhancementAnalysisCompleted,
            this, &AIEnhancementDialog::onAnalysisCompleted);
    connect(m_provider, &IAIProvider::analysisError,
            this, &AIEnhancementDialog::onAnalysisError);

    // Start analysis
    m_provider->analyzeImageForEnhancements(m_imagePath);
}

void AIEnhancementDialog::onAnalysisCompleted(const ImageEnhancementAnalysis& analysis)
{
    m_analysis = analysis;

    m_statusLabel->setText(tr("✓ Analysis completed"));
    m_statusLabel->setStyleSheet("color: green;");
    m_progressBar->setRange(0, 1);
    m_progressBar->setValue(1);

    displaySuggestions(analysis);

    m_applyButton->setEnabled(true);
    m_selectAllButton->setEnabled(true);
    m_deselectAllButton->setEnabled(true);
}

void AIEnhancementDialog::onAnalysisError(const QString& error)
{
    m_statusLabel->setText(tr("✗ Error: %1").arg(error));
    m_statusLabel->setStyleSheet("color: red;");
    m_progressBar->setRange(0, 1);
    m_progressBar->setValue(1);

    QMessageBox::critical(this, tr("AI Analysis Error"),
        tr("Failed to analyze image:\n\n%1").arg(error));
}

void AIEnhancementDialog::displaySuggestions(const ImageEnhancementAnalysis& analysis)
{
    // Display overall assessment
    QString assessmentText = analysis.overallAssessment;
    if (!analysis.technicalAnalysis.isEmpty()) {
        assessmentText += "\n\nTechnical Details:\n" + analysis.technicalAnalysis;
    }
    m_assessmentText->setPlainText(assessmentText);

    // Display suggestions as checkable list items
    m_suggestionsList->clear();

    for (const ImageEnhancementSuggestion& suggestion : analysis.suggestions) {
        QString itemText = QString("%1: %2 (confidence: %3%)\n  Reason: %4")
            .arg(suggestion.operation)
            .arg(suggestion.value)
            .arg(qRound(suggestion.confidence * 100))
            .arg(suggestion.reason);

        QListWidgetItem* item = new QListWidgetItem(itemText, m_suggestionsList);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(suggestion.selected ? Qt::Checked : Qt::Unchecked);
    }

    if (analysis.suggestions.isEmpty()) {
        QListWidgetItem* item = new QListWidgetItem(
            tr("No enhancement suggestions available."), m_suggestionsList);
        item->setFlags(Qt::ItemIsEnabled);
    }
}

void AIEnhancementDialog::onApplyClicked()
{
    QList<ImageEnhancementSuggestion> selectedSuggestions = getSelectedSuggestions();

    if (selectedSuggestions.isEmpty()) {
        QMessageBox::information(this, tr("No Selections"),
            tr("Please select at least one suggestion to apply."));
        return;
    }

    emit applyEnhancements(selectedSuggestions);
    accept();
}

void AIEnhancementDialog::onCancelClicked()
{
    reject();
}

void AIEnhancementDialog::onSelectAllClicked()
{
    for (int i = 0; i < m_suggestionsList->count(); ++i) {
        QListWidgetItem* item = m_suggestionsList->item(i);
        if (item->flags() & Qt::ItemIsUserCheckable) {
            item->setCheckState(Qt::Checked);
        }
    }
}

void AIEnhancementDialog::onDeselectAllClicked()
{
    for (int i = 0; i < m_suggestionsList->count(); ++i) {
        QListWidgetItem* item = m_suggestionsList->item(i);
        if (item->flags() & Qt::ItemIsUserCheckable) {
            item->setCheckState(Qt::Unchecked);
        }
    }
}

QList<ImageEnhancementSuggestion> AIEnhancementDialog::getSelectedSuggestions() const
{
    QList<ImageEnhancementSuggestion> selected;

    for (int i = 0; i < m_suggestionsList->count() && i < m_analysis.suggestions.count(); ++i) {
        QListWidgetItem* item = m_suggestionsList->item(i);
        if (item->checkState() == Qt::Checked) {
            selected.append(m_analysis.suggestions[i]);
        }
    }

    return selected;
}
