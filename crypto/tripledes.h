#ifndef TRIPLEDES_H
#define TRIPLEDES_H

#include <QByteArray>
#include <QString>
#include <QVector>

class TripleDES {
public:
    TripleDES();
    
    // Устанавливает ключи для шифрования/дешифрования
    void setKeys(const QByteArray &key1, const QByteArray &key2, const QByteArray &key3);
    
    // Шифрует данные с использованием Triple DES
    QByteArray encrypt(const QByteArray &data) const;
    
    // Дешифрует данные с использованием Triple DES
    QByteArray decrypt(const QByteArray &encryptedData) const;
    
    // Шифрует строку и возвращает результат в Base64
    QString encryptToBase64(const QString &text) const;
    
    // Дешифрует строку из Base64
    QString decryptFromBase64(const QString &base64Text) const;
    
    // Генерирует случайные ключи
    static QByteArray generateRandomKey();
    
private:
    // Ключи для Triple DES
    QByteArray m_key1;
    QByteArray m_key2;
    QByteArray m_key3;
    
    // Реализация DES (одиночный проход)
    QByteArray desEncrypt(const QByteArray &data, const QByteArray &key) const;
    QByteArray desDecrypt(const QByteArray &data, const QByteArray &key) const;
    
    // Вспомогательные функции для DES
    quint64 initialPermutation(quint64 block) const;
    quint64 finalPermutation(quint64 block) const;
    quint64 feistelFunction(quint64 block, quint64 key) const;
    QVector<quint64> generateSubkeys(quint64 key) const;
};

#endif // TRIPLEDES_H 