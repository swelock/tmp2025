#include "sha384.h"
#include <QCryptographicHash>

SHA384::SHA384() {
    // Пустой конструктор
}

QByteArray SHA384::hash(const QByteArray &data) {
    // Используем встроенную реализацию SHA-384 из Qt
    return QCryptographicHash::hash(data, QCryptographicHash::Sha384);
}

QString SHA384::hashString(const QString &text) {
    QByteArray data = text.toUtf8();
    QByteArray hashResult = hash(data);
    return hashResult.toHex();
}

bool SHA384::verify(const QString &text, const QString &hashHex) {
    QString calculatedHash = hashString(text);
    return (calculatedHash.compare(hashHex, Qt::CaseInsensitive) == 0);
} 