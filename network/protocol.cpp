#include "protocol.h"

namespace Protocol {

Request::Request(RequestType type) : m_type(type) {}

Request::Request(const QJsonObject &json) {
    int typeInt = json["type"].toInt();
    m_type = static_cast<RequestType>(typeInt);
    m_data = json["data"].toObject();
}

QJsonObject Request::toJson() const {
    QJsonObject json;
    json["type"] = static_cast<int>(m_type);
    json["data"] = m_data;
    return json;
}

RequestType Request::type() const {
    return m_type;
}

void Request::setData(const QString &key, const QJsonValue &value) {
    m_data[key] = value;
}

QJsonValue Request::data(const QString &key) const {
    return m_data[key];
}

Response::Response() : m_status(ResponseStatus::SUCCESS) {}

Response::Response(ResponseStatus status) : m_status(status) {}

Response::Response(const QJsonObject &json) {
    int statusInt = json["status"].toInt();
    m_status = static_cast<ResponseStatus>(statusInt);
    m_data = json["data"].toObject();
    m_errorMessage = json["error_message"].toString();
}

QJsonObject Response::toJson() const {
    QJsonObject json;
    json["status"] = static_cast<int>(m_status);
    json["data"] = m_data;
    json["error_message"] = m_errorMessage;
    return json;
}

ResponseStatus Response::status() const {
    return m_status;
}

void Response::setData(const QString &key, const QJsonValue &value) {
    m_data[key] = value;
}

QJsonValue Response::data(const QString &key) const {
    return m_data[key];
}

void Response::setErrorMessage(const QString &message) {
    m_errorMessage = message;
}

QString Response::errorMessage() const {
    return m_errorMessage;
}

} // namespace Protocol 