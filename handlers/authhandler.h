#ifndef AUTHHANDLER_H
#define AUTHHANDLER_H

#include <QObject>
#include "../../common/network/protocol.h"
#include "../database/databasesingleton.h"

class AuthHandler : public QObject {
    Q_OBJECT
    
public:
    explicit AuthHandler(QObject *parent = nullptr);
    
    // Обработка запроса на регистрацию
    Protocol::Response handleRegister(const Protocol::Request &request);
    
    // Обработка запроса на авторизацию
    Protocol::Response handleLogin(const Protocol::Request &request);
    
private:
    // Ссылка на синглтон базы данных
    DatabaseSingleton *m_db;
};

#endif // AUTHHANDLER_H 