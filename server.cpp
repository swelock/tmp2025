#include "server.h"
#include <QJsonDocument>
#include <QDebug>
#include "database/databasesingleton.h"

Server::Server(QObject *parent) : QObject(parent) {
    m_server = new QTcpServer(this);
    m_requestHandler = new RequestHandler(this);
    
    // Подключаем сигналы
    connect(m_server, &QTcpServer::newConnection, this, &Server::onNewConnection);
    
    // Инициализируем базу данных
    DatabaseSingleton::getInstance()->init();
}

Server::~Server() {
    stop();
    DatabaseSingleton::getInstance()->close();
}

bool Server::start(int port) {
    if (m_server->isListening()) {
        return true;
    }
    
    bool success = m_server->listen(QHostAddress::Any, port);
    
    if (success) {
        qDebug() << "Сервер запущен на порту" << port;
    } else {
        qDebug() << "Ошибка запуска сервера:" << m_server->errorString();
    }
    
    return success;
}

void Server::stop() {
    if (m_server->isListening()) {
        // Отключаем всех клиентов
        for (auto client : m_clients.keys()) {
            client->disconnectFromHost();
        }
        
        m_clients.clear();
        m_server->close();
        
        qDebug() << "Сервер остановлен";
    }
}

bool Server::isRunning() const {
    return m_server->isListening();
}

void Server::onNewConnection() {
    QTcpSocket *client = m_server->nextPendingConnection();
    
    // Подключаем сигналы для клиента
    connect(client, &QTcpSocket::readyRead, this, &Server::onReadyRead);
    connect(client, &QTcpSocket::disconnected, this, &Server::onClientDisconnected);
    
    // Добавляем клиента в список
    m_clients[client] = QByteArray();
    
    qDebug() << "Новое подключение:" << client->peerAddress().toString() << ":" << client->peerPort();
}

void Server::onClientDisconnected() {
    QTcpSocket *client = qobject_cast<QTcpSocket*>(sender());
    
    if (client) {
        // Удаляем клиента из списка
        m_clients.remove(client);
        
        qDebug() << "Клиент отключился:" << client->peerAddress().toString() << ":" << client->peerPort();
        
        // Освобождаем ресурсы
        client->deleteLater();
    }
}

void Server::onReadyRead() {
    QTcpSocket *client = qobject_cast<QTcpSocket*>(sender());
    
    if (client) {
        // Добавляем новые данные к буферу клиента
        m_clients[client].append(client->readAll());
        
        // Пытаемся обработать данные как JSON
        QJsonParseError parseError;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(m_clients[client], &parseError);
        
        if (parseError.error == QJsonParseError::NoError) {
            // Очищаем буфер клиента
            m_clients[client].clear();
            
            // Обрабатываем запрос
            Protocol::Request request(jsonDoc.object());
            Protocol::Response response = m_requestHandler->handleRequest(request);
            
            // Отправляем ответ клиенту
            sendResponse(client, response);
        }
    }
}

void Server::sendResponse(QTcpSocket *client, const Protocol::Response &response) {
    if (client && client->isOpen()) {
        // Преобразуем ответ в JSON
        QJsonDocument jsonDoc(response.toJson());
        QByteArray jsonData = jsonDoc.toJson();
        
        // Отправляем данные клиенту
        client->write(jsonData);
        client->flush();
    }
} 