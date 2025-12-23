# Pix3lForge

Pix3lForge is a professional cross-platform desktop image editor built with C++ and Qt 6.9. Designed with AI video generation workflows in mind, it provides powerful preprocessing tools for images that will be used in AI-powered video creation.

![License](https://img.shields.io/badge/license-MIT-blue.svg)
![Qt](https://img.shields.io/badge/Qt-6.9-green.svg)
![Platform](https://img.shields.io/badge/platform-Windows%20%7C%20Linux%20%7C%20macOS-lightgrey.svg)

## Application Icon

<p align="center">
  <img src="resources/icons/icon.png" alt="PixelForge Icon" width="256">
</p>

## Features

### Image Adjustments
- **Basic Adjustments**: Brightness, contrast, saturation, hue, gamma correction
- **Color Adjustments**: Color temperature, exposure, shadows/highlights
- **Advanced Filters**: B&W, Sepia, Vignette, HDR, Sharpen, Blur, Edge detection

### Transformations
- **Rotation**: Rotate images by any angle
- **Flipping**: Horizontal and vertical flip
- **Resizing**: Scale images to desired dimensions
- **Cropping**: Select and crop specific regions

### AI-Powered Enhancement
- **Multi-Provider Support**: LMStudio, OpenRouter, OpenAI, Anthropic
- **Intelligent Enhancement**: AI-driven image quality improvements
- **Customizable Parameters**: Temperature, max tokens, and provider-specific settings
- **Preview Before Apply**: See AI suggestions before committing changes

### Advanced Features
- **Watermarking**: Add text and image watermarks
- **Batch Processing**: Process multiple images with the same settings
- **Preset Management**: Save and load adjustment presets
- **Undo/Redo**: Full command history with unlimited undo/redo
- **Dark Theme**: Modern, unified dark interface with cyan accents
- **Logging**: Comprehensive logging for troubleshooting

## Installation

### Download Pre-built Binaries

Download the latest release for your platform from the [Releases](https://github.com/paolosereno/pix3lforge/releases) page.

### Build from Source

#### Prerequisites

- C++17 compatible compiler
- Qt 6.9 or later
- CMake 3.16 or later
- Git

#### Build Instructions

```bash
# Clone the repository
git clone https://github.com/paolosereno/pix3lforge.git
cd pix3lforge

# Create build directory
mkdir build
cd build

# Configure with CMake
cmake ..

# Build the project
cmake --build .

# Run PixelForge
./Pix3lForge           # Linux/macOS
.\Pix3lForge.exe       # Windows
```

## Usage

### Basic Workflow

1. **Open an Image**: File → Open or drag and drop an image
2. **Apply Adjustments**: Use the properties panel to adjust image parameters
3. **Preview Changes**: See real-time preview of your adjustments
4. **Save Results**: File → Save or Save As to export the edited image

### AI Enhancement

1. **Configure AI Provider**: Settings → AI Settings
   - Choose your provider (LMStudio, OpenRouter, OpenAI, or Anthropic)
   - Enter API credentials (if required)
   - Set enhancement parameters

2. **Enhance Image**: Tools → AI Enhancement
   - Describe desired improvements
   - Preview AI suggestions
   - Apply selected enhancements

### Batch Processing

1. **Load Multiple Images**: File → Batch Processing
2. **Configure Settings**: Apply same adjustments to all images
3. **Process**: Execute batch operation
4. **Export**: Save all processed images

### Keyboard Shortcuts

| Action | Shortcut |
|--------|----------|
| Open Image | `Ctrl+O` |
| Save | `Ctrl+S` |
| Save As | `Ctrl+Shift+S` |
| Undo | `Ctrl+Z` |
| Redo | `Ctrl+Y` |
| Zoom In | `Ctrl++` |
| Zoom Out | `Ctrl+-` |
| Fit to Window | `Ctrl+0` |
| Exit | `Ctrl+Q` |

## Configuration

Settings are automatically saved and include:

- Window geometry and state
- Recent files
- AI provider configuration
- User preferences
- Custom presets

Configuration files are stored in:
- **Windows**: `%APPDATA%/Pix3ltools-lab/PixelForge/`
- **Linux**: `~/.config/Pix3ltools-lab/PixelForge/`
- **macOS**: `~/Library/Preferences/Pix3ltools-lab/PixelForge/`

## Architecture

PixelForge follows a clean, modular architecture:

```
src/
├── main.cpp                    # Application entry point
├── mainwindow.h/cpp           # Main window
├── model/                     # Data models
│   ├── imagedocument.h/cpp   # Image document model
│   └── adjustmentparameters.h # Adjustment parameters
├── widgets/                   # UI widgets
│   └── propertiespanel.h/cpp # Properties editing panel
├── commands/                  # Command pattern (undo/redo)
│   ├── imagecommand.h/cpp    # Base command
│   ├── commandmanager.h/cpp  # Command history
│   └── commandfactory.h/cpp  # Command creation
├── view/                      # View management
│   └── viewmanager.h/cpp     # Viewport handling
├── dialogs/                   # Dialog windows
│   ├── dialogmanager.h/cpp   # Dialog coordination
│   ├── aboutdialog.h/cpp     # About dialog
│   ├── logviewerdialog.h/cpp # Log viewer
│   ├── AISettingsDialog.h/cpp    # AI configuration
│   └── AIEnhancementDialog.h/cpp # AI enhancement
├── ai/                        # AI enhancement system
│   ├── IAIProvider.h         # Provider interface
│   ├── AIProviderFactory.h/cpp   # Provider factory
│   ├── providers/            # AI provider implementations
│   │   ├── LMStudioProvider.h/cpp
│   │   ├── OpenRouterProvider.h/cpp
│   │   ├── OpenAIProvider.h/cpp
│   │   └── AnthropicProvider.h/cpp
│   ├── EnhancementPromptBuilder.h/cpp
│   ├── EnhancementResponseParser.h/cpp
│   ├── ImageEncoder.h/cpp
│   └── RetryPolicy.h
├── settings/                  # Settings management
│   └── settingsmanager.h/cpp # Persistent settings
├── logging/                   # Logging system
│   └── logger.h/cpp          # Application logger
└── pix3ltheme.h/cpp          # Unified dark theme
```

## Contributing

We welcome contributions! Please see [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Author

**Pix3ltools-lab**
- GitHub: [@Pix3ltools-lab](https://github.com/Pix3ltools-lab)
- X (Twitter): [@pix3ltools](https://x.com/pix3ltools)

## Acknowledgments

- Built with [Qt 6.9](https://www.qt.io/)
- Part of the Pix3ltools-lab ecosystem
- AI enhancement powered by multiple providers

## Support

- **Issues**: [GitHub Issues](https://github.com/paolosereno/pix3lforge/issues)
- **Discussions**: [GitHub Discussions](https://github.com/paolosereno/pix3lforge/discussions)
- **Documentation**: [Wiki](https://github.com/paolosereno/pix3lforge/wiki)

---

**Made with Qt by Pix3ltools-lab**
