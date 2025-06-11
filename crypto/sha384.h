#ifndef SHA384_H
#define SHA384_H

#include <QByteArray>
#include <QString>

class SHA384 {
public:
    SHA384();
    
    // Вычисляет хеш SHA-384 для данных
    static QByteArray hash(const QByteArray &data);
    
    // Вычисляет хеш SHA-384 для строки и возвращает его в виде hex-строки
    static QString hashString(const QString &text);
    
    // Проверяет, соответствует ли строка заданному хешу
    static bool verify(const QString &text, const QString &hashHex);
};

#endif // SHA384_H 