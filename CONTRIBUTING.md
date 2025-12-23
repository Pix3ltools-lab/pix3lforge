# Contributing to Pix3lForge

Thank you for your interest in contributing to Pix3lForge! This document provides guidelines for contributing to the project.

## 🎯 How Can I Contribute?

### Reporting Bugs

Before creating bug reports, please check existing issues to avoid duplicates. When creating a bug report, include:

- **Description**: Clear and concise description of the bug
- **Steps to Reproduce**: Detailed steps to reproduce the behavior
- **Expected Behavior**: What you expected to happen
- **Actual Behavior**: What actually happened
- **Screenshots**: If applicable, add screenshots
- **Environment**:
  - OS: Windows/Linux/macOS version
  - Qt version: 6.9 or later
  - Image format: What image file caused the issue
  - Log output: From Help → View Logs

### Suggesting Features

Feature suggestions are welcome! Please include:

- **Use Case**: Why is this feature needed?
- **Description**: Clear description of the feature
- **Mockups**: If applicable, add mockups or wireframes
- **Alternatives**: Have you considered any alternatives?

### Pull Requests

1. **Fork** the repository
2. **Create** a new branch (`git checkout -b feature/amazing-feature`)
3. **Make** your changes
4. **Test** thoroughly
5. **Commit** with clear messages (`git commit -m 'Add amazing feature'`)
6. **Push** to your branch (`git push origin feature/amazing-feature`)
7. **Open** a Pull Request

## 🛠️ Development Setup

### Prerequisites

- **Qt 6.9** or later
- **CMake 3.16** or later
- **C++17** compatible compiler
- **Git** for version control
- **Qt Creator** (recommended IDE)

### Building from Source

```bash
# Clone repository
git clone https://github.com/paolosereno/pix3lforge.git
cd pix3lforge

# Create build directory
mkdir build
cd build

# Configure and build
cmake ..
cmake --build .

# Run
./Pix3lForge  # Linux/macOS
.\Pix3lForge.exe  # Windows
```

## 📝 Code Style Guidelines

### C++ Code

- **Indentation**: 4 spaces (no tabs)
- **Naming**:
  - Classes: `PascalCase` (e.g., `MainWindow`, `ImageProcessor`)
  - Methods: `camelCase` (e.g., `loadImage`, `applyFilter`)
  - Private members: `camelCase` (e.g., `imageProcessor`, `currentDocument`)
  - Constants: `UPPER_SNAKE_CASE` (e.g., `MAX_RECENT_FILES`)
- **Braces**: Opening brace on same line
  ```cpp
  void myFunction() {
      // code here
  }
  ```
- **Comments**: Use `//` for single-line, `/* */` for multi-line
- **Header Guards**: Use `#pragma once`

### Qt Specific

- **Signals/Slots**: Use new-style connections
  ```cpp
  connect(button, &QPushButton::clicked, this, &MainWindow::onButtonClicked);
  ```
- **Memory Management**: Use parent-child ownership when possible
- **Logging**: Use centralized `Logger` class
  ```cpp
  LOG_INFO("Image loaded successfully");
  LOG_ERROR(QString("Failed to load: %1").arg(filePath));
  ```

### Commit Messages

Follow conventional commits format:

```
<type>(<scope>): <subject>

<body>

<footer>
```

**Types**:
- `feat`: New feature
- `fix`: Bug fix
- `docs`: Documentation changes
- `style`: Code style changes (formatting)
- `refactor`: Code refactoring
- `test`: Adding tests
- `chore`: Build/tooling changes

**Examples**:
```
feat(ai): Add multi-provider AI enhancement support

- Implement LMStudio, OpenRouter, OpenAI, and Anthropic providers
- Add AI settings dialog for configuration
- Create enhancement dialog with preview
- Support temperature and max tokens parameters

Closes #12
```

```
fix(image): Fix memory leak in batch processing

Release QImage objects properly after processing.
This prevents memory accumulation during batch operations.

Fixes #18
```

## 🧪 Testing

### Manual Testing Checklist

Before submitting a PR, ensure:

- [ ] Application builds without warnings
- [ ] All existing features still work
- [ ] New feature works as expected
- [ ] No memory leaks
- [ ] Dark theme remains consistent
- [ ] Settings are persisted correctly
- [ ] Logs show expected output

### Test Cases

When adding new features, test:

1. **Normal usage**: Feature works as designed
2. **Edge cases**: Empty inputs, large images, etc.
3. **Error handling**: Invalid inputs, missing files, etc.
4. **Performance**: No slowdowns with large files
5. **UI responsiveness**: No freezing or blocking

## 📚 Documentation

When adding features:

- Update `README.md` with new functionality
- Include inline code comments for complex logic
- Update keyboard shortcuts table if adding shortcuts
- Document new AI providers or enhancement options

## 🔧 Architecture Guidelines

PixelForge follows a clean architecture with:

- **Model**: `ImageDocument`, `AdjustmentParameters`
- **View**: `ViewManager`, `PropertiesPanel`
- **Commands**: Command pattern for undo/redo
- **Dialogs**: `DialogManager` for centralized dialog handling
- **Settings**: `SettingsManager` for persistence
- **AI**: Provider pattern for multiple AI backends

When adding code, respect these boundaries and patterns.

## 🚀 Release Process

(For maintainers)

1. Update version in `CMakeLists.txt`
2. Update `CHANGELOG.md` (if exists)
3. Tag release: `git tag -a v1.0.0 -m "Release version 1.0.0"`
4. Push tags: `git push --tags`
5. Create GitHub release with binaries

## ❓ Questions?

If you have questions:

1. Check existing issues and documentation
2. Open a new issue with `[Question]` prefix
3. Be specific and include context

## 📄 License

By contributing, you agree that your contributions will be licensed under the MIT License.

---

**Thank you for contributing to PixelForge!** 🎨✨

