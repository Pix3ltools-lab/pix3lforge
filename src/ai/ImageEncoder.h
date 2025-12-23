#ifndef IMAGEENCODER_H
#define IMAGEENCODER_H

#include <QString>

/**
 * Utility class for encoding images to base64
 * Responsibility: Convert images to base64 and determine MIME types
 */
class ImageEncoder
{
public:
    /**
     * Convert an image file to base64 string
     * @param imagePath Path to the image file
     * @return Base64 encoded string, empty string on error
     */
    static QString imageToBase64(const QString& imagePath);

    /**
     * Determine the MIME type from file extension
     * @param imagePath Path to the image file
     * @return MIME type (e.g., "image/jpeg", "image/png")
     */
    static QString getMimeType(const QString& imagePath);

    /**
     * Create a data URL from image path
     * @param imagePath Path to the image file
     * @return Data URL string (data:image/jpeg;base64,...)
     */
    static QString createDataUrl(const QString& imagePath);
};

#endif // IMAGEENCODER_H
