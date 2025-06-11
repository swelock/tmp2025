#ifndef REQUESTHANDLER_H
#define REQUESTHANDLER_H

#include <QObject>
#include <QImage>
#include "../../common/network/protocol.h"
#include "authhandler.h"

class RequestHandler : public QObject {
    Q_OBJECT
    
public:
    explicit RequestHandler(QObject *parent = nullptr);
    
    // Обработка запроса
    Protocol::Response handleRequest(const Protocol::Request &request);
    
private:
    // Обработчик авторизации
    AuthHandler *m_authHandler;
    
    // Обработка запроса на шифрование Triple DES
    Protocol::Response handleTripleDESEncrypt(const Protocol::Request &request);
    
    // Обработка запроса на дешифрование Triple DES
    Protocol::Response handleTripleDESDecrypt(const Protocol::Request &request);
    
    // Обработка запроса на решение уравнения методом хорд
    Protocol::Response handleChordMethodSolve(const Protocol::Request &request);
    
    // Обработка запроса на скрытие сообщения в изображении
    Protocol::Response handleSteganographyHide(const Protocol::Request &request);
    
    // Обработка запроса на извлечение сообщения из изображения
    Protocol::Response handleSteganographyExtract(const Protocol::Request &request);
};

#endif // REQUESTHANDLER_H 