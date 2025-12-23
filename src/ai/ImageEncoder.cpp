#include "ImageEncoder.h"
#include <QFile>
#include <QFileInfo>
#include <QImage>
#include <QBuffer>
#include <QDebug>

QString ImageEncoder::imageToBase64(const QString& imagePath)
{
    QFile file(imagePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "ImageEncoder: Cannot open file:" << imagePath;
        return QString();
    }

    QByteArray fileData = file.readAll();
    file.close();

    return QString::fromLatin1(fileData.toBase64());
}

QString ImageEncoder::getMimeType(const QString& imagePath)
{
    QString extension = QFileInfo(imagePath).suffix().toLower();

    if (extension == "jpg" || extension == "jpeg")
        return "image/jpeg";
    else if (extension == "png")
        return "image/png";
    else if (extension == "gif")
        return "image/gif";
    else if (extension == "bmp")
        return "image/bmp";
    else if (extension == "webp")
        return "image/webp";
    else if (extension == "tiff" || extension == "tif")
        return "image/tiff";
    else
        return "image/jpeg";  // Default fallback
}

QString ImageEncoder::createDataUrl(const QString& imagePath)
{
    QString base64 = imageToBase64(imagePath);
    if (base64.isEmpty())
        return QString();

    QString mimeType = getMimeType(imagePath);
    return QString("data:%1;base64,%2").arg(mimeType, base64);
}
