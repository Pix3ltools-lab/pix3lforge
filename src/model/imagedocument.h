#ifndef IMAGEDOCUMENT_H
#define IMAGEDOCUMENT_H

#include <QObject>
#include <QImage>
#include <QString>

/**
 * @class ImageDocument
 * @brief Manages the document state including the current image and file path
 *
 * ImageDocument is responsible for:
 * - Loading and saving images from/to disk
 * - Managing the current and original image state
 * - Tracking document modifications
 *
 * This class follows the Single Responsibility Principle by handling only
 * document-level concerns, separating file I/O and state management from the UI.
 */
class ImageDocument : public QObject
{
    Q_OBJECT

public:
    explicit ImageDocument(QObject *parent = nullptr);

    // File operations
    bool load(const QString &filePath);
    bool save();
    bool saveAs(const QString &filePath);

    // State management
    QImage getCurrentImage() const;
    QImage getOriginalImage() const;
    void setCurrentImage(const QImage &image);
    void setOriginalImage(const QImage &image);

    // Direct access for undo commands (returns pointer to internal image)
    QImage* currentImagePtr();

    // Properties
    QString filePath() const;
    bool isModified() const;
    bool isEmpty() const;

    // Image dimensions
    int width() const;
    int height() const;
    int depth() const;

    // Clear document
    void clear();

signals:
    void imageChanged(const QImage &newImage);
    void originalImageChanged(const QImage &newImage);
    void filePathChanged(const QString &path);
    void modifiedChanged(bool modified);
    void loaded(const QString &filePath);
    void saved(const QString &filePath);
    void errorOccurred(const QString &error);

private:
    QImage m_currentImage;
    QImage m_originalImage;
    QString m_filePath;
    bool m_modified;

    // Validation
    bool validateImage(const QImage &image) const;
    QString generateErrorMessage(const QString &operation, const QString &details) const;
    void setModified(bool modified);
};

#endif // IMAGEDOCUMENT_H
