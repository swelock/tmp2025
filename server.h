#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QMap>
#include "handlers/requesthandler.h"
#include "../common/network/protocol.h"

class Server : public QObject {
    Q_OBJECT
    
public:
    explicit Server(QObject *parent = nullptr);
    ~Server();
    
    // Запуск сервера на указанном порту
    bool start(int port = 12345);
    
    // Остановка сервера
    void stop();
    
    // Проверка, запущен ли сервер
    bool isRunning() const;
    
    // Получение количества подключенных клиентов
    int getClientCount() const { return m_clients.size(); }
    
private slots:
    // Обработка нового подключения
    void onNewConnection();
    
    // Обработка отключения клиента
    void onClientDisconnected();
    
    // Обработка данных от клиента
    void onReadyRead();
    
private:
    // TCP-сервер
    QTcpServer *m_server;
    
    // Обработчик запросов
    RequestHandler *m_requestHandler;
    
    // Список подключенных клиентов
    QMap<QTcpSocket*, QByteArray> m_clients;
    
    // Отправка ответа клиенту
    void sendResponse(QTcpSocket *client, const Protocol::Response &response);
};

#endif // SERVER_H 