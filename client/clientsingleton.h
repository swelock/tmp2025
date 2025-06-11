#ifndef CLIENTSINGLETON_H
#define CLIENTSINGLETON_H

#include <QObject>
#include <QTcpSocket>
#include <QMutex>
#include <QQueue>
#include <QJsonDocument>
#include <QImage>
#include "../../common/network/protocol.h"

// Предварительное объявление класса QEventLoop
class QEventLoop;

class ClientSingleton : public QObject {
    Q_OBJECT
    
public:
    // Получение экземпляра синглтона
    static ClientSingleton* getInstance();
    
    // Подключение к серверу
    bool connectToServer(const QString &host, int port);
    
    // Отключение от сервера
    void disconnectFromServer();
    
    // Проверка, подключен ли клиент к серверу
    bool isConnected() const;
    
    // Отправка запроса на сервер и ожидание ответа
    Protocol::Response sendRequest(const Protocol::Request &request);
    
    // Авторизация пользователя
    Protocol::Response login(const QString &username, const QString &password);
    
    // Регистрация пользователя
    Protocol::Response registerUser(const QString &username, const QString &password);
    
    // Шифрование текста с использованием Triple DES
    Protocol::Response encryptTripleDES(const QString &text, const QByteArray &key1, const QByteArray &key2, const QByteArray &key3);
    
    // Дешифрование текста с использованием Triple DES
    Protocol::Response decryptTripleDES(const QString &encryptedText, const QByteArray &key1, const QByteArray &key2, const QByteArray &key3);
    
    // Решение уравнения методом хорд
    Protocol::Response solveEquation(const QString &equation, double a, double b, double epsilon = 0.0001, int maxIterations = 100);
    
    // Скрытие сообщения в изображении
    Protocol::Response hideMessage(const QImage &image, const QString &message);
    
    // Извлечение сообщения из изображения
    Protocol::Response extractMessage(const QImage &image);
    
signals:
    // Сигнал об изменении состояния подключения
    void connectionStateChanged(bool connected);
    
    // Сигнал о получении ответа от сервера
    void responseReceived(const Protocol::Response &response);
    
private slots:
    // Обработка подключения к серверу
    void onConnected();
    
    // Обработка отключения от сервера
    void onDisconnected();
    
    // Обработка ошибки сокета
    void onError(QAbstractSocket::SocketError socketError);
    
    // Обработка данных от сервера
    void onReadyRead();
    
private:
    // Приватный конструктор (синглтон)
    explicit ClientSingleton(QObject *parent = nullptr);
    
    // Запрещаем копирование и присваивание
    ClientSingleton(const ClientSingleton&) = delete;
    ClientSingleton& operator=(const ClientSingleton&) = delete;
    
    // Статический экземпляр синглтона
    static ClientSingleton* instance;
    
    // Мьютекс для потокобезопасности
    static QMutex mutex;
    
    // TCP-сокет для связи с сервером
    QTcpSocket *m_socket;
    
    // Буфер для данных от сервера
    QByteArray m_buffer;
    
    // Очередь запросов
    QQueue<Protocol::Request> m_requestQueue;
    
    // Последний полученный ответ
    Protocol::Response m_lastResponse;
    
    // Флаг ожидания ответа
    bool m_waitingForResponse;
    
    // Событие для синхронизации запросов и ответов
    QEventLoop *m_eventLoop;
};

#endif // CLIENTSINGLETON_H 