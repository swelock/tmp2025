#ifndef STEGANOGRAPHY_H
#define STEGANOGRAPHY_H

#include <QString>
#include <QImage>

class Steganography {
public:
    Steganography();
    
    // Результат операции стеганографии
    struct Result {
        bool success;
        QString message;
        QImage image;
    };
    
    // Скрывает сообщение в изображении
    static Result hideMessage(const QImage &sourceImage, const QString &message);
    
    // Извлекает сообщение из изображения
    static Result extractMessage(const QImage &image);
    
    // Сохраняет изображение в файл
    static bool saveImage(const QImage &image, const QString &filePath);
    
    // Загружает изображение из файла
    static QImage loadImage(const QString &filePath, bool &success);
    
private:
    // Максимальная длина сообщения, которое можно скрыть в изображении
    static int maxMessageLength(const QImage &image);
    
    // Преобразует строку в массив битов
    static QVector<bool> stringToBits(const QString &message);
    
    // Преобразует массив битов в строку
    static QString bitsToString(const QVector<bool> &bits);
};

#endif // STEGANOGRAPHY_H 