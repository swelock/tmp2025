#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <QObject>
#include <QString>
#include <QJsonObject>
#include <QJsonDocument>

namespace Protocol {

enum class RequestType {
    AUTH_LOGIN,
    AUTH_REGISTER,
    TRIPLE_DES_ENCRYPT,
    TRIPLE_DES_DECRYPT,
    CHORD_METHOD_SOLVE,
    STEGANOGRAPHY_HIDE,
    STEGANOGRAPHY_EXTRACT
};

enum class ResponseStatus {
    SUCCESS,
    ERROR
};

class Request {
public:
    Request(RequestType type);
    Request(const QJsonObject &json);
    
    QJsonObject toJson() const;
    RequestType type() const;
    
    void setData(const QString &key, const QJsonValue &value);
    QJsonValue data(const QString &key) const;
    
private:
    RequestType m_type;
    QJsonObject m_data;
};

class Response {
public:
    Response();
    Response(ResponseStatus status);
    Response(const QJsonObject &json);
    
    QJsonObject toJson() const;
    ResponseStatus status() const;
    
    void setData(const QString &key, const QJsonValue &value);
    QJsonValue data(const QString &key) const;
    
    void setErrorMessage(const QString &message);
    QString errorMessage() const;
    
private:
    ResponseStatus m_status;
    QJsonObject m_data;
    QString m_errorMessage;
};

} // namespace Protocol

#endif // PROTOCOL_H 