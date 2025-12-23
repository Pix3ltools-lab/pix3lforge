// pix3ltheme.cpp
#include "pix3ltheme.h"

void Pix3lTheme::applyTheme(QApplication *app, Theme theme)
{
    if (theme == Dark) {
        applyDarkTheme(app);
    } else {
        applyLightTheme(app);
    }
}

void Pix3lTheme::applyDarkTheme(QApplication *app)
{
    // Set the dark palette
    app->setPalette(getDarkPalette());

    // Apply global stylesheet
    app->setStyleSheet(getStyleSheet(Dark));
}

void Pix3lTheme::applyLightTheme(QApplication *app)
{
    // Set the light palette
    app->setPalette(getLightPalette());

    // Apply global stylesheet
    app->setStyleSheet(getStyleSheet(Light));
}

QPalette Pix3lTheme::getDarkPalette()
{
    QPalette darkPalette;

    // Window colors
    darkPalette.setColor(QPalette::Window, QColor(Colors::WindowBackground));
    darkPalette.setColor(QPalette::WindowText, QColor(Colors::WindowText));

    // Base colors (for input widgets)
    darkPalette.setColor(QPalette::Base, QColor(Colors::BaseBackground));
    darkPalette.setColor(QPalette::AlternateBase, QColor(Colors::AlternateBase));
    darkPalette.setColor(QPalette::Text, QColor(Colors::Text));

    // Tooltip colors
    darkPalette.setColor(QPalette::ToolTipBase, QColor(Colors::ToolTipBackground));
    darkPalette.setColor(QPalette::ToolTipText, QColor(Colors::WindowText));

    // Bright text
    darkPalette.setColor(QPalette::BrightText, QColor(Colors::BrightText));

    // Button colors
    darkPalette.setColor(QPalette::Button, QColor(Colors::Button));
    darkPalette.setColor(QPalette::ButtonText, QColor(Colors::ButtonText));

    // Highlight colors (selection)
    darkPalette.setColor(QPalette::Highlight, QColor(Colors::Highlight));
    darkPalette.setColor(QPalette::HighlightedText, QColor(Colors::HighlightedText));

    // Link colors
    darkPalette.setColor(QPalette::Link, QColor(Colors::Link));
    darkPalette.setColor(QPalette::LinkVisited, QColor(Colors::LinkVisited));

    // Disabled colors
    darkPalette.setColor(QPalette::Disabled, QPalette::WindowText, QColor(Colors::DisabledText));
    darkPalette.setColor(QPalette::Disabled, QPalette::Text, QColor(Colors::DisabledText));
    darkPalette.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(Colors::DisabledText));
    darkPalette.setColor(QPalette::Disabled, QPalette::Base, QColor("#2A2A2A"));
    darkPalette.setColor(QPalette::Disabled, QPalette::Button, QColor("#2A2A2A"));

    // Placeholder text
    darkPalette.setColor(QPalette::PlaceholderText, QColor(Colors::PlaceholderText));

    return darkPalette;
}

QPalette Pix3lTheme::getLightPalette()
{
    QPalette lightPalette;

    // Window colors
    lightPalette.setColor(QPalette::Window, QColor(LightColors::WindowBackground));
    lightPalette.setColor(QPalette::WindowText, QColor(LightColors::WindowText));

    // Base colors (for input widgets)
    lightPalette.setColor(QPalette::Base, QColor(LightColors::BaseBackground));
    lightPalette.setColor(QPalette::AlternateBase, QColor(LightColors::AlternateBase));
    lightPalette.setColor(QPalette::Text, QColor(LightColors::Text));

    // Tooltip colors
    lightPalette.setColor(QPalette::ToolTipBase, QColor(LightColors::ToolTipBackground));
    lightPalette.setColor(QPalette::ToolTipText, QColor(LightColors::WindowText));

    // Bright text
    lightPalette.setColor(QPalette::BrightText, QColor(LightColors::BrightText));

    // Button colors
    lightPalette.setColor(QPalette::Button, QColor(LightColors::Button));
    lightPalette.setColor(QPalette::ButtonText, QColor(LightColors::ButtonText));

    // Highlight colors (selection)
    lightPalette.setColor(QPalette::Highlight, QColor(LightColors::Highlight));
    lightPalette.setColor(QPalette::HighlightedText, QColor(LightColors::HighlightedText));

    // Link colors
    lightPalette.setColor(QPalette::Link, QColor(LightColors::Link));
    lightPalette.setColor(QPalette::LinkVisited, QColor(LightColors::LinkVisited));

    // Disabled colors
    lightPalette.setColor(QPalette::Disabled, QPalette::WindowText, QColor(LightColors::DisabledText));
    lightPalette.setColor(QPalette::Disabled, QPalette::Text, QColor(LightColors::DisabledText));
    lightPalette.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(LightColors::DisabledText));
    lightPalette.setColor(QPalette::Disabled, QPalette::Base, QColor("#F0F0F0"));
    lightPalette.setColor(QPalette::Disabled, QPalette::Button, QColor("#F0F0F0"));

    // Placeholder text
    lightPalette.setColor(QPalette::PlaceholderText, QColor(LightColors::PlaceholderText));

    return lightPalette;
}

QString Pix3lTheme::getStyleSheet(Theme theme)
{
    return QString(R"(
        /* ========== Global Styles ========== */
        QWidget {
            font-family: "Segoe UI", "Arial", sans-serif;
            font-size: 9pt;
        }

        /* ========== Main Window ========== */
        QMainWindow {
            background-color: %1;
        }

        QMainWindow::separator {
            background: %2;
            width: 1px;
            height: 1px;
        }

        /* ========== Menu Bar ========== */
        QMenuBar {
            background-color: %1;
            color: %3;
            border-bottom: 1px solid %2;
        }

        QMenuBar::item {
            background-color: transparent;
            padding: 4px 12px;
        }

        QMenuBar::item:selected {
            background-color: %4;
        }

        QMenuBar::item:pressed {
            background-color: %5;
        }

        /* ========== Menus ========== */
        QMenu {
            background-color: %6;
            color: %3;
            border: 1px solid %2;
        }

        QMenu::item {
            padding: 6px 25px 6px 10px;
        }

        QMenu::item:selected {
            background-color: %5;
        }

        QMenu::separator {
            height: 1px;
            background: %2;
            margin: 4px 0px;
        }

        /* ========== Tool Bar ========== */
        QToolBar {
            background-color: %1;
            border-bottom: 1px solid %2;
            spacing: 3px;
            padding: 4px;
        }

        QToolBar::separator {
            background: %2;
            width: 1px;
            margin: 4px;
        }

        QToolButton {
            background-color: transparent;
            border: 1px solid transparent;
            border-radius: 3px;
            padding: 5px;
            margin: 2px;
            color: %3;
        }

        QToolButton:hover {
            background-color: %4;
            border: 1px solid %7;
        }

        QToolButton:pressed {
            background-color: %5;
            border: 1px solid %5;
        }

        QToolButton:checked {
            background-color: %5;
            border: 1px solid %5;
        }

        /* ========== Push Buttons ========== */
        QPushButton {
            background-color: %8;
            color: %9;
            border: 1px solid %2;
            border-radius: 3px;
            padding: 6px 12px;
            min-width: 60px;
        }

        QPushButton:hover {
            background-color: %4;
            border: 1px solid %7;
        }

        QPushButton:pressed {
            background-color: %5;
            border: 1px solid %5;
        }

        QPushButton:disabled {
            background-color: %13;
            color: %10;
            border: 1px solid %13;
        }

        /* ========== Line Edit ========== */
        QLineEdit, QSpinBox, QTimeEdit, QDoubleSpinBox {
            background-color: %6;
            color: %3;
            border: 1px solid %2;
            border-radius: 3px;
            padding: 4px 6px;
            selection-background-color: %5;
            selection-color: %11;
        }

        QLineEdit:focus, QSpinBox:focus, QTimeEdit:focus, QDoubleSpinBox:focus {
            border: 1px solid %5;
        }

        QLineEdit:disabled, QSpinBox:disabled, QTimeEdit:disabled, QDoubleSpinBox:disabled {
            background-color: %13;
            color: %10;
        }

        /* ========== Spin Box Buttons ========== */
        QSpinBox::up-button, QSpinBox::down-button,
        QTimeEdit::up-button, QTimeEdit::down-button,
        QDoubleSpinBox::up-button, QDoubleSpinBox::down-button {
            width: 16px;
            background-color: %8;
            border: 1px solid %2;
            border-radius: 2px;
        }

        QSpinBox::up-button:hover, QSpinBox::down-button:hover,
        QTimeEdit::up-button:hover, QTimeEdit::down-button:hover,
        QDoubleSpinBox::up-button:hover, QDoubleSpinBox::down-button:hover {
            background-color: %4;
            border: 1px solid %7;
        }

        QSpinBox::up-button:pressed, QSpinBox::down-button:pressed,
        QTimeEdit::up-button:pressed, QTimeEdit::down-button:pressed,
        QDoubleSpinBox::up-button:pressed, QDoubleSpinBox::down-button:pressed {
            background-color: %5;
        }

        /* ========== Combo Box ========== */
        QComboBox {
            background-color: %6;
            color: %3;
            border: 1px solid %2;
            border-radius: 3px;
            padding: 4px 6px;
        }

        QComboBox:hover {
            border: 1px solid %7;
        }

        QComboBox:focus {
            border: 1px solid %5;
        }

        QComboBox::drop-down {
            border: none;
            padding-right: 6px;
        }

        QComboBox QAbstractItemView {
            background-color: %6;
            color: %3;
            border: 1px solid %2;
            selection-background-color: %5;
            selection-color: %11;
        }

        /* ========== Slider ========== */
        QSlider::groove:horizontal {
            background: %2;
            height: 6px;
            border-radius: 3px;
        }

        QSlider::handle:horizontal {
            background: %5;
            width: 14px;
            margin: -4px 0;
            border-radius: 7px;
        }

        QSlider::handle:horizontal:hover {
            background: #26DAF0;
        }

        QSlider::sub-page:horizontal {
            background: %5;
            border-radius: 3px;
        }

        /* ========== Status Bar ========== */
        QStatusBar {
            background-color: %1;
            color: %3;
            border-top: 1px solid %2;
        }

        QStatusBar::item {
            border: none;
        }

        QStatusBar QLabel {
            color: %3;
            padding: 2px 8px;
        }

        /* ========== Dock Widget ========== */
        QDockWidget {
            titlebar-close-icon: url(close.png);
            titlebar-normal-icon: url(undock.png);
            color: %3;
        }

        QDockWidget::title {
            background-color: %1;
            border: 1px solid %2;
            padding: 6px;
        }

        QDockWidget::close-button, QDockWidget::float-button {
            background: transparent;
            border: none;
            padding: 2px;
        }

        QDockWidget::close-button:hover, QDockWidget::float-button:hover {
            background: %4;
        }

        /* ========== Scroll Bar ========== */
        QScrollBar:vertical {
            background: %1;
            width: 12px;
            margin: 0px;
        }

        QScrollBar::handle:vertical {
            background: %2;
            min-height: 20px;
            border-radius: 6px;
        }

        QScrollBar::handle:vertical:hover {
            background: %7;
        }

        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            height: 0px;
        }

        QScrollBar:horizontal {
            background: %1;
            height: 12px;
            margin: 0px;
        }

        QScrollBar::handle:horizontal {
            background: %2;
            min-width: 20px;
            border-radius: 6px;
        }

        QScrollBar::handle:horizontal:hover {
            background: %7;
        }

        QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal {
            width: 0px;
        }

        /* ========== Group Box ========== */
        QGroupBox {
            border: 1px solid %2;
            border-radius: 5px;
            margin-top: 12px;
            padding-top: 6px;
            font-weight: bold;
            color: %3;
        }

        QGroupBox::title {
            subcontrol-origin: margin;
            subcontrol-position: top left;
            padding: 0 5px;
            color: %5;
        }

        /* ========== Tab Widget ========== */
        QTabWidget::pane {
            border: 1px solid %2;
            background-color: %1;
        }

        QTabBar::tab {
            background-color: %8;
            color: %3;
            border: 1px solid %2;
            border-bottom: none;
            padding: 6px 12px;
            margin-right: 2px;
        }

        QTabBar::tab:selected {
            background-color: %6;
            border-bottom: 2px solid %5;
        }

        QTabBar::tab:hover {
            background-color: %4;
        }

        /* ========== Radio Button & Check Box ========== */
        QRadioButton, QCheckBox {
            color: %3;
            spacing: 8px;
        }

        QRadioButton::indicator, QCheckBox::indicator {
            width: 16px;
            height: 16px;
        }

        QRadioButton::indicator:unchecked, QCheckBox::indicator:unchecked {
            background-color: %6;
            border: 1px solid %2;
        }

        QRadioButton::indicator:checked, QCheckBox::indicator:checked {
            background-color: %5;
            border: 1px solid %5;
        }

        QRadioButton::indicator:hover, QCheckBox::indicator:hover {
            border: 1px solid %7;
        }

        /* ========== Tooltip ========== */
        QToolTip {
            background-color: %12;
            color: %3;
            border: 1px solid %5;
            padding: 4px;
            border-radius: 3px;
        }

        /* ========== Dialog ========== */
        QDialog {
            background-color: %1;
        }

        QDialogButtonBox QPushButton {
            min-width: 80px;
        }

        /* ========== Text Edit & List Widget ========== */
        QTextEdit, QPlainTextEdit, QListWidget {
            background-color: %6;
            color: %3;
            border: 1px solid %2;
            selection-background-color: %5;
            selection-color: %11;
        }

        QListWidget::item:hover {
            background-color: %4;
        }

        QListWidget::item:selected {
            background-color: %5;
            color: %11;
        }

        /* ========== Progress Bar ========== */
        QProgressBar {
            background-color: %6;
            border: 1px solid %2;
            border-radius: 3px;
            text-align: center;
            color: %3;
        }

        QProgressBar::chunk {
            background-color: %5;
            border-radius: 2px;
        }

        /* ========== Undo View (QListView for History) ========== */
        QUndoView {
            background-color: %6;
            color: %3;
            border: 1px solid %2;
            selection-background-color: %5;
            selection-color: %11;
        }

        QUndoView::item:hover {
            background-color: %4;
        }

        QUndoView::item:selected {
            background-color: %5;
            color: %11;
        }
    )").arg(theme == Dark ? DarkColors::WindowBackground : LightColors::WindowBackground)       // %1
       .arg(theme == Dark ? DarkColors::Border : LightColors::Border)                            // %2
       .arg(theme == Dark ? DarkColors::WindowText : LightColors::WindowText)                    // %3
       .arg(theme == Dark ? DarkColors::ButtonHover : LightColors::ButtonHover)                  // %4
       .arg(theme == Dark ? DarkColors::Highlight : LightColors::Highlight)                      // %5 (Teal)
       .arg(theme == Dark ? DarkColors::BaseBackground : LightColors::BaseBackground)            // %6
       .arg(theme == Dark ? DarkColors::FocusBorder : LightColors::FocusBorder)                  // %7 (Teal)
       .arg(theme == Dark ? DarkColors::Button : LightColors::Button)                            // %8
       .arg(theme == Dark ? DarkColors::ButtonText : LightColors::ButtonText)                    // %9
       .arg(theme == Dark ? DarkColors::DisabledText : LightColors::DisabledText)                // %10
       .arg(theme == Dark ? DarkColors::HighlightedText : LightColors::HighlightedText)          // %11
       .arg(theme == Dark ? DarkColors::ToolTipBackground : LightColors::ToolTipBackground)      // %12
       .arg(theme == Dark ? "#2A2A2A" : "#F0F0F0");                                              // %13 (disabled bg)
}
