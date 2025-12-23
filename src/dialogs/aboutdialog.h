#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

/**
 * @brief About dialog displaying application information
 *
 * AboutDialog provides information about Pix3lForge with application details,
 * features list, and copyright information.
 */
class AboutDialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief Constructor for AboutDialog
     * @param parent Parent widget
     */
    explicit AboutDialog(QWidget *parent = nullptr);

    /**
     * @brief Destructor
     */
    ~AboutDialog() = default;

private:
    void setupUI();
    void setupContent();
    void setupButtonBox();

    // UI Components
    QVBoxLayout* m_mainLayout;
    QLabel* m_logoLabel;
    QLabel* m_appInfoLabel;
    QTextEdit* m_featuresText;
    QLabel* m_copyrightLabel;
    QHBoxLayout* m_buttonLayout;
    QPushButton* m_closeButton;
};

#endif // ABOUTDIALOG_H
