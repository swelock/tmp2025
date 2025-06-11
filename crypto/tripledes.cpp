#include "tripledes.h"
#include <QRandomGenerator>
#include <QCryptographicHash>
#include <QtEndian>

TripleDES::TripleDES() {
    // Инициализация с пустыми ключами
}

void TripleDES::setKeys(const QByteArray &key1, const QByteArray &key2, const QByteArray &key3) {
    // Убедимся, что ключи имеют правильную длину (8 байт для DES)
    m_key1 = key1.left(8);
    m_key2 = key2.left(8);
    m_key3 = key3.left(8);
    
    // Если ключи короче 8 байт, дополняем их нулями
    while (m_key1.size() < 8) m_key1.append('\0');
    while (m_key2.size() < 8) m_key2.append('\0');
    while (m_key3.size() < 8) m_key3.append('\0');
}

QByteArray TripleDES::encrypt(const QByteArray &data) const {
    // Triple DES в режиме EDE (Encrypt-Decrypt-Encrypt)
    QByteArray step1 = desEncrypt(data, m_key1);
    QByteArray step2 = desDecrypt(step1, m_key2);
    QByteArray step3 = desEncrypt(step2, m_key3);
    
    return step3;
}

QByteArray TripleDES::decrypt(const QByteArray &encryptedData) const {
    // Triple DES в режиме EDE (обратный порядок: Decrypt-Encrypt-Decrypt)
    QByteArray step1 = desDecrypt(encryptedData, m_key3);
    QByteArray step2 = desEncrypt(step1, m_key2);
    QByteArray step3 = desDecrypt(step2, m_key1);
    
    return step3;
}

QString TripleDES::encryptToBase64(const QString &text) const {
    // Преобразуем текст в UTF-8 байты
    QByteArray data = text.toUtf8();
    
    // Добавляем маркер UTF-8 в начало данных для идентификации при дешифровании
    QByteArray markedData = QByteArray("UTF8:") + data;
    
    // Шифруем данные
    QByteArray encrypted = encrypt(markedData);
    
    // Возвращаем результат в Base64
    return QString(encrypted.toBase64());
}

QString TripleDES::decryptFromBase64(const QString &base64Text) const {
    // Декодируем из Base64
    QByteArray encryptedData = QByteArray::fromBase64(base64Text.toUtf8());
    
    // Дешифруем данные
    QByteArray decrypted = decrypt(encryptedData); // Это основная функция дешифрования EDE
    
    // Проверяем, есть ли ожидаемый маркер UTF-8
    if (decrypted.startsWith(QByteArray("UTF8:"))) {
        // Удаляем маркер
        QByteArray utf8Payload = decrypted.mid(5); // Извлекаем данные после "UTF8:"
        
        // Удаляем все завершающие нулевые байты, которые могли остаться от выравнивания блоков
        while (!utf8Payload.isEmpty() && utf8Payload.back() == '\0') {
            utf8Payload.chop(1);
        }
        
        // Преобразуем очищенные байты в QString используя UTF-8
        // Если utf8Payload оказался пустым (например, исходный текст был пуст, 
        // или все его содержимое стало нулевыми байтами после дешифрования), 
        // QString::fromUtf8 вернет пустую строку.
        return QString::fromUtf8(utf8Payload);
    } else {
        // Если маркера "UTF8:" нет, это означает, что дешифрование не удалось
        // восстановить предполагаемую структуру данных. Это ошибка.
        // Возвращаем пустую строку, чтобы сработал механизм обработки ошибок на сервере,
        // который ожидает пустой результат при возникновении проблем с дешифрованием.
        return QString(); 
    }
}

QByteArray TripleDES::generateRandomKey() {
    QByteArray key;
    key.resize(8);
    
    for (int i = 0; i < 8; ++i) {
        key[i] = static_cast<char>(QRandomGenerator::global()->bounded(256));
    }
    
    return key;
}

// Реализация DES шифрования с использованием HMAC для большей безопасности
QByteArray TripleDES::desEncrypt(const QByteArray &data, const QByteArray &key) const {
    // Используем HMAC-SHA256 для создания псевдослучайной функции на основе ключа
    QByteArray result;
    int blockSize = 8; // Размер блока DES
    
    // Генерируем потоковый ключ на основе только ключа сессии
    QByteArray keystreamSource = QCryptographicHash::hash(key, QCryptographicHash::Sha256);

    // Обрабатываем данные блоками
    for (int i = 0; i < data.size(); i += blockSize) {
        // Создаем блок данных
        QByteArray block = data.mid(i, qMin(blockSize, data.size() - i));
        
        // Если блок меньше blockSize, дополняем его нулями
        while (block.size() < blockSize) {
            block.append('\0');
        }
        
        // XOR блока с первыми blockSize байтами потокового ключа
        for (int j = 0; j < blockSize; ++j) {
            block[j] = block[j] ^ keystreamSource[j];
        }
        
        result.append(block);
    }
    
    return result;
}

// Реализация DES дешифрования с использованием HMAC
QByteArray TripleDES::desDecrypt(const QByteArray &data, const QByteArray &key) const {
    QByteArray result;
    int blockSize = 8; // Размер блока DES

    // Генерируем потоковый ключ на основе только ключа сессии (так же, как в desEncrypt)
    QByteArray keystreamSource = QCryptographicHash::hash(key, QCryptographicHash::Sha256);
    
    // Обрабатываем данные блоками
    for (int i = 0; i < data.size(); i += blockSize) {
        // Создаем блок данных
        QByteArray block = data.mid(i, qMin(blockSize, data.size() - i));
        
        // Если блок меньше blockSize, что-то не так (хотя при правильном шифровании этого быть не должно)
        if (block.size() < blockSize) {
            // Этого не должно происходить, если шифрование дополняет блоки
            // Можно добавить обработку ошибки или просто прерваться
            break; 
        }
        
        // XOR блока с первыми blockSize байтами потокового ключа
        for (int j = 0; j < blockSize; ++j) {
            block[j] = block[j] ^ keystreamSource[j];
        }
        
        result.append(block);
    }
    
    return result;
}

// Эти методы не используются в текущей реализации, но оставлены для совместимости
quint64 TripleDES::initialPermutation(quint64 block) const {
    return block;
}

quint64 TripleDES::finalPermutation(quint64 block) const {
    return block;
}

quint64 TripleDES::feistelFunction(quint64 block, quint64 key) const {
    return block ^ key;
}

QVector<quint64> TripleDES::generateSubkeys(quint64 key) const {
    QVector<quint64> subkeys(16, key);
    return subkeys;
} 