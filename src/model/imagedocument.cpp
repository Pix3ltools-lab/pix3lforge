#include "imagedocument.h"
#include "../logging/logger.h"
#include <QImageReader>
#include <QImageWriter>
#include <QDir>
#include <QFileInfo>

ImageDocument::ImageDocument(QObject *parent)
    : QObject(parent)
    , m_modified(false)
{
}

bool ImageDocument::load(const QString &filePath)
{
    LOG_INFO(QString("Loading image: %1").arg(filePath));

    if (filePath.isEmpty()) {
        LOG_ERROR("Load failed: file path is empty");
        emit errorOccurred(tr("Cannot load: file path is empty"));
        return false;
    }

    QImageReader reader(filePath);
    reader.setAutoTransform(true);
    const QImage newImage = reader.read();

    if (newImage.isNull()) {
        LOG_ERROR(QString("Load failed: %1 - %2").arg(filePath).arg(reader.errorString()));
        QString error = generateErrorMessage(tr("load"),
                                            reader.errorString());
        emit errorOccurred(error);
        return false;
    }

    if (!validateImage(newImage)) {
        LOG_ERROR(QString("Load failed: invalid image format - %1").arg(filePath));
        emit errorOccurred(tr("Invalid image format or dimensions"));
        return false;
    }

    LOG_INFO(QString("Image loaded: %1x%2, %3-bit, %4 KB")
             .arg(newImage.width())
             .arg(newImage.height())
             .arg(newImage.depth())
             .arg(QFileInfo(filePath).size() / 1024));

    m_currentImage = newImage;
    m_originalImage = newImage;
    m_filePath = filePath;
    setModified(false);

    emit imageChanged(m_currentImage);
    emit originalImageChanged(m_originalImage);
    emit filePathChanged(m_filePath);
    emit loaded(filePath);

    return true;
}

bool ImageDocument::save()
{
    if (m_filePath.isEmpty()) {
        emit errorOccurred(tr("Cannot save: no file path specified"));
        return false;
    }

    return saveAs(m_filePath);
}

bool ImageDocument::saveAs(const QString &filePath)
{
    LOG_INFO(QString("Saving image to: %1").arg(filePath));

    if (filePath.isEmpty()) {
        LOG_ERROR("Save failed: file path is empty");
        emit errorOccurred(tr("Cannot save: file path is empty"));
        return false;
    }

    if (m_currentImage.isNull()) {
        LOG_ERROR("Save failed: no image loaded");
        emit errorOccurred(tr("Cannot save: no image loaded"));
        return false;
    }

    // Determine quality based on file format
    int quality = -1; // Use default for most formats

    // For JPEG, we could use a default quality
    // (in MainWindow, user is prompted for quality)
    QFileInfo fileInfo(filePath);
    QString suffix = fileInfo.suffix().toLower();
    if (suffix == "jpg" || suffix == "jpeg") {
        quality = 90; // Default JPEG quality
    }

    if (!m_currentImage.save(filePath, nullptr, quality)) {
        LOG_ERROR(QString("Save failed: %1 - Could not write to file").arg(filePath));
        QString error = generateErrorMessage(tr("save"),
                                            tr("Could not write to file"));
        emit errorOccurred(error);
        return false;
    }

    LOG_INFO(QString("Image saved successfully: %1 (quality: %2)").arg(filePath).arg(quality));

    m_filePath = filePath;
    setModified(false);

    emit filePathChanged(m_filePath);
    emit saved(filePath);

    return true;
}

QImage ImageDocument::getCurrentImage() const
{
    return m_currentImage;
}

QImage ImageDocument::getOriginalImage() const
{
    return m_originalImage;
}

void ImageDocument::setCurrentImage(const QImage &image)
{
    if (image.isNull()) {
        return;
    }

    m_currentImage = image;
    setModified(true);
    emit imageChanged(m_currentImage);
}

void ImageDocument::setOriginalImage(const QImage &image)
{
    if (image.isNull()) {
        return;
    }

    m_originalImage = image;
    emit originalImageChanged(m_originalImage);
}

QImage* ImageDocument::currentImagePtr()
{
    return &m_currentImage;
}

QString ImageDocument::filePath() const
{
    return m_filePath;
}

bool ImageDocument::isModified() const
{
    return m_modified;
}

bool ImageDocument::isEmpty() const
{
    return m_currentImage.isNull();
}

int ImageDocument::width() const
{
    return m_currentImage.width();
}

int ImageDocument::height() const
{
    return m_currentImage.height();
}

int ImageDocument::depth() const
{
    return m_currentImage.depth();
}

void ImageDocument::clear()
{
    m_currentImage = QImage();
    m_originalImage = QImage();
    m_filePath.clear();
    setModified(false);

    emit imageChanged(m_currentImage);
    emit originalImageChanged(m_originalImage);
    emit filePathChanged(m_filePath);
}

bool ImageDocument::validateImage(const QImage &image) const
{
    if (image.isNull()) {
        return false;
    }

    // Check for reasonable dimensions
    if (image.width() <= 0 || image.height() <= 0) {
        return false;
    }

    // Could add more validation here
    // e.g., max dimensions, supported formats, etc.

    return true;
}

QString ImageDocument::generateErrorMessage(const QString &operation,
                                           const QString &details) const
{
    QString fileName = m_filePath.isEmpty()
                      ? tr("(unknown)")
                      : QDir::toNativeSeparators(m_filePath);

    return tr("Cannot %1 %2: %3")
           .arg(operation)
           .arg(fileName)
           .arg(details);
}

void ImageDocument::setModified(bool modified)
{
    if (m_modified != modified) {
        m_modified = modified;
        emit modifiedChanged(m_modified);
    }
}
