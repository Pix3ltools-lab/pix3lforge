#include "aboutdialog.h"
#include <QApplication>
#include <QFont>
#include <QPixmap>

AboutDialog::AboutDialog(QWidget *parent)
    : QDialog(parent)
    , m_mainLayout(nullptr)
    , m_logoLabel(nullptr)
    , m_appInfoLabel(nullptr)
    , m_featuresText(nullptr)
    , m_copyrightLabel(nullptr)
    , m_buttonLayout(nullptr)
    , m_closeButton(nullptr)
{
    setupUI();
}

void AboutDialog::setupUI()
{
    setWindowTitle(tr("About Pix3lForge"));
    setModal(true);
    resize(550, 600);

    m_mainLayout = new QVBoxLayout(this);

    setupContent();
    setupButtonBox();

    m_mainLayout->addLayout(m_buttonLayout);
}

void AboutDialog::setupContent()
{
    // Logo/Icon area
    m_logoLabel = new QLabel;
    m_logoLabel->setAlignment(Qt::AlignCenter);
    m_logoLabel->setMinimumHeight(140);
    m_logoLabel->setMaximumHeight(140);

    // Try to load application icon from resources
    QPixmap iconPixmap(":/icons/icons/icon.png");
    if (!iconPixmap.isNull()) {
        // Scale the icon to an appropriate size (128x128 for better visibility)
        iconPixmap = iconPixmap.scaled(128, 128, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        m_logoLabel->setPixmap(iconPixmap);
    } else {
        // Fallback to styled text with colored logo if icon can't be loaded
        m_logoLabel->setStyleSheet("QLabel { background-color: #2c3e50; border-radius: 8px; padding: 20px; }");
        m_logoLabel->setTextFormat(Qt::RichText);
        m_logoLabel->setText(QString::fromUtf8(
            "<span style='font-size: 28px; font-weight: bold; color: white;'>"
            "Pix<span style='color: #ef4444;'>3</span><span style='color: #6366f1;'>l</span>Forge"
            "</span>"
        ));
    }
    m_mainLayout->addWidget(m_logoLabel);

    // Application information
    m_appInfoLabel = new QLabel;
    m_appInfoLabel->setAlignment(Qt::AlignCenter);
    m_appInfoLabel->setTextFormat(Qt::RichText);
    QFont headerFont = m_appInfoLabel->font();
    headerFont.setPointSize(12);
    headerFont.setBold(true);
    m_appInfoLabel->setFont(headerFont);

    QString appInfo = tr("<h2>Pix<span style='color: #ef4444;'>3</span><span style='color: #6366f1;'>l</span>Forge v1.0</h2>"
                        "<p><b>Professional Image Editor</b></p>"
                        "<p>A powerful Qt 6 application for editing and enhancing images</p>"
                        "<p>Built with Qt %1</p>"
                        "<p>Compiled on %2 %3</p>")
                     .arg(QT_VERSION_STR)
                     .arg(__DATE__)
                     .arg(__TIME__);
    m_appInfoLabel->setText(appInfo);
    m_appInfoLabel->setOpenExternalLinks(true);
    m_mainLayout->addWidget(m_appInfoLabel);

    // Features list
    m_featuresText = new QTextEdit;
    m_featuresText->setReadOnly(true);
    m_featuresText->setMaximumHeight(250);

    QString features = tr("<h3>Key Features:</h3>"
                         "<ul>"
                         "<li><b>Real-time Adjustments:</b> Live preview with brightness, contrast, saturation, and more</li>"
                         "<li><b>Auto-Enhance:</b> Intelligent automatic image enhancement with one click</li>"
                         "<li><b>Professional Filters:</b> Black & White, Sepia, Vignette, Sharpen, Blur, Edge Detection</li>"
                         "<li><b>Transform Tools:</b> Rotate, flip, resize, and crop images</li>"
                         "<li><b>Full Undo/Redo:</b> Complete history with visual command list</li>"
                         "<li><b>Advanced Color:</b> Color temperature, exposure, shadows, and highlights control</li>"
                         "<li><b>Watermarks:</b> Add text or image watermarks to your photos</li>"
                         "<li><b>Modern UI:</b> Clean interface with Material Design SVG icons</li>"
                         "<li><b>Resizable Panels:</b> Customizable workspace layout</li>"
                         "</ul>");
    m_featuresText->setHtml(features);
    m_mainLayout->addWidget(m_featuresText);

    // Copyright and links
    m_copyrightLabel = new QLabel;
    m_copyrightLabel->setAlignment(Qt::AlignCenter);
    m_copyrightLabel->setTextFormat(Qt::RichText);
    m_copyrightLabel->setText(tr("<p>Part of <b>Pix<span style='color: #ef4444;'>3</span><span style='color: #6366f1;'>l</span>Tools</b> Suite</p>"
                              "<p>© 2025 Pix3lTools. All rights reserved.</p>"
                              "<p>Licensed under the MIT License</p>"));
    m_mainLayout->addWidget(m_copyrightLabel);

    m_mainLayout->addStretch();
}

void AboutDialog::setupButtonBox()
{
    m_buttonLayout = new QHBoxLayout;

    m_closeButton = new QPushButton(tr("Close"));
    m_closeButton->setDefault(true);
    connect(m_closeButton, &QPushButton::clicked, this, &QDialog::accept);

    m_buttonLayout->addStretch();
    m_buttonLayout->addWidget(m_closeButton);
}
