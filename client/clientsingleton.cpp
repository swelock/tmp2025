#include "clientsingleton.h"
#include <QEventLoop>
#include <QTimer>
#include <QBuffer>
#include "../../common/crypto/sha384.h"

// Инициализация статических членов
ClientSingleton* ClientSingleton::instance = nullptr;
QMutex ClientSingleton::mutex;

ClientSingleton::ClientSingleton(QObject *parent) : QObject(parent), m_waitingForResponse(false), m_eventLoop(nullptr) {
    m_socket = new QTcpSocket(this);
    
    // Подключаем сигналы сокета
    connect(m_socket, &QTcpSocket::connected, this, &ClientSingleton::onConnected);
    connect(m_socket, &QTcpSocket::disconnected, this, &ClientSingleton::onDisconnected);
    connect(m_socket, &QTcpSocket::readyRead, this, &ClientSingleton::onReadyRead);
    
    // Исправленное подключение сигнала ошибки для Qt6
    #if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        connect(m_socket, &QAbstractSocket::errorOccurred, this, &ClientSingleton::onError);
    #else
        connect(m_socket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), this, &ClientSingleton::onError);
    #endif
}

ClientSingleton* ClientSingleton::getInstance() {
    // Потокобезопасное создание синглтона
    QMutexLocker locker(&mutex);
    
    if (!instance) {
        instance = new ClientSingleton();
    }
    
    return instance;
}

bool ClientSingleton::connectToServer(const QString &host, int port) {
    if (m_socket->state() == QAbstractSocket::ConnectedState) {
        return true;
    }
    
    m_socket->connectToHost(host, port);
    
    // Ждем подключения с таймаутом 5 секунд
    return m_socket->waitForConnected(5000);
}

void ClientSingleton::disconnectFromServer() {
    if (m_socket->state() == QAbstractSocket::ConnectedState) {
        m_socket->disconnectFromHost();
    }
}

bool ClientSingleton::isConnected() const {
    return m_socket->state() == QAbstractSocket::ConnectedState;
}

Protocol::Response ClientSingleton::sendRequest(const Protocol::Request &request) {
    if (!isConnected()) {
        Protocol::Response response(Protocol::ResponseStatus::ERROR);
        response.setErrorMessage("Нет подключения к серверу");
        return response;
    }
    
    // Преобразуем запрос в JSON
    QJsonDocument jsonDoc(request.toJson());
    QByteArray jsonData = jsonDoc.toJson();
    
    // Очищаем буфер
    m_buffer.clear();
    
    // Устанавливаем флаг ожидания ответа
    m_waitingForResponse = true;
    
    // Отправляем данные на сервер
    m_socket->write(jsonData);
    m_socket->flush();
    
    // Создаем цикл событий для ожидания ответа
    QEventLoop eventLoop;
    m_eventLoop = &eventLoop;
    
    // Таймаут 10 секунд
    QTimer timer;
    timer.setSingleShot(true);
    connect(&timer, &QTimer::timeout, &eventLoop, &QEventLoop::quit);
    timer.start(10000);
    
    // Ждем ответа или таймаута
    eventLoop.exec();
    m_eventLoop = nullptr;
    
    // Проверяем, получили ли мы ответ
    if (m_waitingForResponse) {
        m_waitingForResponse = false;
        Protocol::Response response(Protocol::ResponseStatus::ERROR);
        response.setErrorMessage("Таймаут ожидания ответа от сервера");
        return response;
    }
    
    return m_lastResponse;
}

Protocol::Response ClientSingleton::login(const QString &username, const QString &password) {
    Protocol::Request request(Protocol::RequestType::AUTH_LOGIN);
    request.setData("username", username);
    request.setData("password", password);
    
    return sendRequest(request);
}

Protocol::Response ClientSingleton::registerUser(const QString &username, const QString &password) {
    Protocol::Request request(Protocol::RequestType::AUTH_REGISTER);
    request.setData("username", username);
    request.setData("password", password);
    
    return sendRequest(request);
}

Protocol::Response ClientSingleton::encryptTripleDES(const QString &text, const QByteArray &key1, const QByteArray &key2, const QByteArray &key3) {
    Protocol::Request request(Protocol::RequestType::TRIPLE_DES_ENCRYPT);
    request.setData("text", text);
    request.setData("key1", QString(key1.toBase64()));
    request.setData("key2", QString(key2.toBase64()));
    request.setData("key3", QString(key3.toBase64()));
    
    return sendRequest(request);
}

Protocol::Response ClientSingleton::decryptTripleDES(const QString &encryptedText, const QByteArray &key1, const QByteArray &key2, const QByteArray &key3) {
    Protocol::Request request(Protocol::RequestType::TRIPLE_DES_DECRYPT);
    request.setData("encrypted_text", encryptedText);
    request.setData("key1", QString(key1.toBase64()));
    request.setData("key2", QString(key2.toBase64()));
    request.setData("key3", QString(key3.toBase64()));
    
    return sendRequest(request);
}

Protocol::Response ClientSingleton::solveEquation(const QString &equation, double a, double b, double epsilon, int maxIterations) {
    Protocol::Request request(Protocol::RequestType::CHORD_METHOD_SOLVE);
    request.setData("equation", equation);
    request.setData("a", a);
    request.setData("b", b);
    request.setData("epsilon", epsilon);
    request.setData("max_iterations", maxIterations);
    
    return sendRequest(request);
}

Protocol::Response ClientSingleton::hideMessage(const QImage &image, const QString &message) {
    Protocol::Request request(Protocol::RequestType::STEGANOGRAPHY_HIDE);
    
    // Сохраняем изображение в формате PNG
    QByteArray imageData;
    QBuffer buffer(&imageData);
    buffer.open(QIODevice::WriteOnly);
    image.save(&buffer, "PNG");
    
    request.setData("image", QString(imageData.toBase64()));
    request.setData("message", message);
    
    return sendRequest(request);
}

Protocol::Response ClientSingleton::extractMessage(const QImage &image) {
    Protocol::Request request(Protocol::RequestType::STEGANOGRAPHY_EXTRACT);
    
    // Сохраняем изображение в формате PNG
    QByteArray imageData;
    QBuffer buffer(&imageData);
    buffer.open(QIODevice::WriteOnly);
    image.save(&buffer, "PNG");
    
    request.setData("image", QString(imageData.toBase64()));
    
    return sendRequest(request);
}

void ClientSingleton::onConnected() {
    emit connectionStateChanged(true);
}

void ClientSingleton::onDisconnected() {
    emit connectionStateChanged(false);
}

void ClientSingleton::onError(QAbstractSocket::SocketError socketError) {
    Q_UNUSED(socketError);
    
    // Если есть активный цикл событий, завершаем его
    if (m_eventLoop) {
        m_eventLoop->quit();
    }
}

void ClientSingleton::onReadyRead() {
    // Добавляем новые данные к буферу
    m_buffer.append(m_socket->readAll());
    
    // Пытаемся обработать данные как JSON
    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(m_buffer, &parseError);
    
    if (parseError.error == QJsonParseError::NoError) {
        // Очищаем буфер
        m_buffer.clear();
        
        // Обрабатываем ответ
        m_lastResponse = Protocol::Response(jsonDoc.object());
        m_waitingForResponse = false;
        
        // Отправляем сигнал о получении ответа
        emit responseReceived(m_lastResponse);
        
        // Если есть активный цикл событий, завершаем его
        if (m_eventLoop) {
            m_eventLoop->quit();
        }
    }
} 