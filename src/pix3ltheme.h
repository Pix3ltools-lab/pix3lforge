// pix3ltheme.h
#ifndef PIX3LTHEME_H
#define PIX3LTHEME_H

#include <QPalette>
#include <QApplication>
#include <QColor>

class Pix3lTheme
{
public:
    enum Theme {
        Dark,
        Light
    };

    // Pix3lTools Professional Dark Theme Colors
    struct DarkColors {
        // Background colors
        static constexpr const char* WindowBackground = "#1E1E1E";       // Main window background
        static constexpr const char* BaseBackground = "#252526";         // Widget base background
        static constexpr const char* AlternateBase = "#2D2D30";          // Alternate rows
        static constexpr const char* ToolTipBackground = "#2D2D30";      // Tooltip background

        // Foreground colors
        static constexpr const char* WindowText = "#CCCCCC";             // Normal text
        static constexpr const char* Text = "#CCCCCC";                   // Input text
        static constexpr const char* BrightText = "#FFFFFF";             // Bright text
        static constexpr const char* DisabledText = "#6B6B6B";           // Disabled text
        static constexpr const char* PlaceholderText = "#767676";        // Placeholder text

        // Accent colors (Teal/Cyan - Pix3lTools brand)
        static constexpr const char* Highlight = "#00BCD4";              // Selection/highlight (teal)
        static constexpr const char* HighlightedText = "#000000";        // Text on highlight
        static constexpr const char* Link = "#4FC3F7";                   // Hyperlinks (light blue)
        static constexpr const char* LinkVisited = "#9C27B0";            // Visited links (purple)

        // Button colors
        static constexpr const char* Button = "#3F3F46";                 // Button background
        static constexpr const char* ButtonText = "#CCCCCC";             // Button text
        static constexpr const char* ButtonHover = "#4A4A52";            // Button hover
        static constexpr const char* ButtonPressed = "#00BCD4";          // Button pressed (teal)

        // Border colors
        static constexpr const char* Border = "#3F3F46";                 // Default borders
        static constexpr const char* FocusBorder = "#00BCD4";            // Focus indicator (teal)

        // Status colors
        static constexpr const char* Success = "#4CAF50";                // Success green
        static constexpr const char* Warning = "#FF9800";                // Warning orange
        static constexpr const char* Error = "#F44336";                  // Error red
        static constexpr const char* Info = "#2196F3";                   // Info blue
    };

    // Pix3lTools Professional Light Theme Colors
    struct LightColors {
        // Background colors
        static constexpr const char* WindowBackground = "#F5F5F5";       // Main window background
        static constexpr const char* BaseBackground = "#FFFFFF";         // Widget base background
        static constexpr const char* AlternateBase = "#FAFAFA";          // Alternate rows
        static constexpr const char* ToolTipBackground = "#FFFDE7";      // Tooltip background

        // Foreground colors
        static constexpr const char* WindowText = "#212121";             // Normal text
        static constexpr const char* Text = "#212121";                   // Input text
        static constexpr const char* BrightText = "#000000";             // Bright text
        static constexpr const char* DisabledText = "#BDBDBD";           // Disabled text
        static constexpr const char* PlaceholderText = "#9E9E9E";        // Placeholder text

        // Accent colors (Teal/Cyan - Pix3lTools brand)
        static constexpr const char* Highlight = "#00BCD4";              // Selection/highlight (teal)
        static constexpr const char* HighlightedText = "#FFFFFF";        // Text on highlight
        static constexpr const char* Link = "#0097A7";                   // Hyperlinks (dark teal)
        static constexpr const char* LinkVisited = "#7B1FA2";            // Visited links (dark purple)

        // Button colors
        static constexpr const char* Button = "#E0E0E0";                 // Button background
        static constexpr const char* ButtonText = "#212121";             // Button text
        static constexpr const char* ButtonHover = "#D5D5D5";            // Button hover
        static constexpr const char* ButtonPressed = "#00BCD4";          // Button pressed (teal)

        // Border colors
        static constexpr const char* Border = "#BDBDBD";                 // Default borders
        static constexpr const char* FocusBorder = "#00BCD4";            // Focus indicator (teal)

        // Status colors
        static constexpr const char* Success = "#4CAF50";                // Success green
        static constexpr const char* Warning = "#FF9800";                // Warning orange
        static constexpr const char* Error = "#F44336";                  // Error red
        static constexpr const char* Info = "#2196F3";                   // Info blue
    };

    // Legacy alias for backward compatibility
    using Colors = DarkColors;

    // Apply theme to the application
    static void applyTheme(QApplication *app, Theme theme);

    // Apply the Pix3lTools dark theme to the application (legacy)
    static void applyDarkTheme(QApplication *app);

    // Apply the Pix3lTools light theme to the application
    static void applyLightTheme(QApplication *app);

    // Get the dark theme palette
    static QPalette getDarkPalette();

    // Get the light theme palette
    static QPalette getLightPalette();

    // Get global stylesheet for widgets
    static QString getStyleSheet(Theme theme);

private:
    Pix3lTheme() = delete; // Static class, no instances
};

#endif // PIX3LTHEME_H
