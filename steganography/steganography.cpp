#include "steganography.h"
#include <QFile>

Steganography::Steganography() {
    // Пустой конструктор
}

Steganography::Result Steganography::hideMessage(const QImage &sourceImage, const QString &message) {
    Result result;
    result.success = false;
    
    // Проверка, что изображение не пустое
    if (sourceImage.isNull()) {
        result.message = "Изображение не загружено";
        return result;
    }
    
    // Проверка, что сообщение не пустое
    if (message.isEmpty()) {
        result.message = "Сообщение пустое";
        return result;
    }
    
    // Проверка, что сообщение поместится в изображение
    int maxLength = maxMessageLength(sourceImage);
    if (message.length() > maxLength) {
        result.message = QString("Сообщение слишком длинное. Максимальная длина: %1 символов").arg(maxLength);
        return result;
    }
    
    // Проверка, что изображение достаточно большое для хранения сообщения
    int requiredPixels = 32 + message.length() * 8; // 32 бита для длины + 8 бит на символ
    if (sourceImage.width() * sourceImage.height() < requiredPixels) {
        result.message = "Изображение слишком маленькое для скрытия сообщения";
        return result;
    }
    
    // Преобразуем сообщение в биты
    QVector<bool> messageBits = stringToBits(message);
    
    // Создаем копию изображения для модификации
    QImage modifiedImage = sourceImage.convertToFormat(QImage::Format_ARGB32);
    
    // Записываем длину сообщения в первые 32 бита (4 байта)
    int messageLength = message.length();
    for (int i = 0; i < 32; ++i) {
        int x = i % modifiedImage.width();
        int y = i / modifiedImage.width();
        
        QColor pixel = modifiedImage.pixelColor(x, y);
        int r = pixel.red();
        
        // Изменяем младший бит красного компонента
        r = (r & 0xFE) | ((messageLength >> i) & 1);
        
        pixel.setRed(r);
        modifiedImage.setPixelColor(x, y, pixel);
    }
    
    // Записываем биты сообщения в изображение
    for (int i = 0; i < messageBits.size(); ++i) {
        int pixelIndex = i + 32; // Смещение на 32 бита (длина сообщения)
        int x = pixelIndex % modifiedImage.width();
        int y = pixelIndex / modifiedImage.width();
        
        QColor pixel = modifiedImage.pixelColor(x, y);
        int r = pixel.red();
        
        // Изменяем младший бит красного компонента
        r = (r & 0xFE) | (messageBits[i] ? 1 : 0);
        
        pixel.setRed(r);
        modifiedImage.setPixelColor(x, y, pixel);
    }
    
    result.success = true;
    result.message = "Сообщение успешно скрыто";
    result.image = modifiedImage;
    
    return result;
}

Steganography::Result Steganography::extractMessage(const QImage &image) {
    Result result;
    result.success = false;
    
    // Проверка, что изображение не пустое
    if (image.isNull()) {
        result.message = "Изображение не загружено";
        return result;
    }
    
    // Извлекаем длину сообщения из первых 32 бит
    int messageLength = 0;
    for (int i = 0; i < 32; ++i) {
        int x = i % image.width();
        int y = i / image.width();
        
        if (x >= image.width() || y >= image.height()) {
            result.message = "Изображение слишком маленькое для извлечения сообщения";
            return result;
        }
        
        QColor pixel = image.pixelColor(x, y);
        int bit = pixel.red() & 1;
        
        messageLength |= (bit << i);
    }
    
    // Проверка валидности длины сообщения
    int maxLength = maxMessageLength(image);
    if (messageLength <= 0 || messageLength > maxLength) {
        result.message = "В изображении нет скрытого сообщения или оно повреждено";
        return result;
    }
    
    // Извлекаем биты сообщения
    QVector<bool> messageBits;
    messageBits.reserve(messageLength * 8); // 8 бит на символ
    
    for (int i = 0; i < messageLength * 8; ++i) {
        int pixelIndex = i + 32; // Смещение на 32 бита (длина сообщения)
        int x = pixelIndex % image.width();
        int y = pixelIndex / image.width();
        
        if (x >= image.width() || y >= image.height()) {
            result.message = "Изображение слишком маленькое для извлечения всего сообщения";
            return result;
        }
        
        QColor pixel = image.pixelColor(x, y);
        int bit = pixel.red() & 1;
        
        messageBits.append(bit == 1);
    }
    
    // Преобразуем биты в строку
    QString extractedMessage = bitsToString(messageBits);
    
    // Проверяем, что извлеченное сообщение содержит только печатные символы
    bool validMessage = true;
    for (const QChar &ch : extractedMessage) {
        if (!ch.isPrint() && !ch.isSpace()) {
            validMessage = false;
            break;
        }
    }
    
    if (!validMessage) {
        result.message = "Извлеченное сообщение содержит недопустимые символы";
        result.success = false;
    } else {
        result.success = true;
        result.message = extractedMessage;
        result.image = image;
    }
    
    return result;
}

bool Steganography::saveImage(const QImage &image, const QString &filePath) {
    return image.save(filePath);
}

QImage Steganography::loadImage(const QString &filePath, bool &success) {
    QImage image(filePath);
    success = !image.isNull();
    return image;
}

int Steganography::maxMessageLength(const QImage &image) {
    // 1 бит на пиксель, минус 32 бита для хранения длины сообщения
    // делим на 8, так как 8 бит на символ
    return (image.width() * image.height() - 32) / 8;
}

QVector<bool> Steganography::stringToBits(const QString &message) {
    QVector<bool> bits;
    QByteArray utf8Data = message.toUtf8();
    
    for (char byte : utf8Data) {
        for (int i = 0; i < 8; ++i) {
            bits.append((byte >> i) & 1);
        }
    }
    
    return bits;
}

QString Steganography::bitsToString(const QVector<bool> &bits) {
    QByteArray bytes;
    
    for (int i = 0; i < bits.size(); i += 8) {
        char byte = 0;
        for (int j = 0; j < 8 && i + j < bits.size(); ++j) {
            if (bits[i + j]) {
                byte |= (1 << j);
            }
        }
        bytes.append(byte);
    }
    
    return QString::fromUtf8(bytes);
} 