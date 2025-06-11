#include "authhandler.h"
#include "../../common/crypto/sha384.h"

AuthHandler::AuthHandler(QObject *parent) : QObject(parent) {
    m_db = DatabaseSingleton::getInstance();
}

Protocol::Response AuthHandler::handleRegister(const Protocol::Request &request) {
    // Получаем данные из запроса
    QString username = request.data("username").toString();
    QString password = request.data("password").toString();
    
    // Проверка на пустые поля
    if (username.isEmpty() || password.isEmpty()) {
        Protocol::Response response(Protocol::ResponseStatus::ERROR);
        response.setErrorMessage("Имя пользователя и пароль не могут быть пустыми");
        return response;
    }
    
    // Хешируем пароль с использованием SHA-384
    QString passwordHash = SHA384::hashString(password);
    
    // Регистрируем пользователя в базе данных
    bool success = m_db->registerUser(username, passwordHash);
    
    if (success) {
        // Успешная регистрация
        Protocol::Response response(Protocol::ResponseStatus::SUCCESS);
        response.setData("message", "Регистрация успешно завершена");
        return response;
    } else {
        // Ошибка регистрации
        Protocol::Response response(Protocol::ResponseStatus::ERROR);
        response.setErrorMessage(m_db->lastError());
        return response;
    }
}

Protocol::Response AuthHandler::handleLogin(const Protocol::Request &request) {
    // Получаем данные из запроса
    QString username = request.data("username").toString();
    QString password = request.data("password").toString();
    
    // Проверка на пустые поля
    if (username.isEmpty() || password.isEmpty()) {
        Protocol::Response response(Protocol::ResponseStatus::ERROR);
        response.setErrorMessage("Имя пользователя и пароль не могут быть пустыми");
        return response;
    }
    
    // Хешируем пароль с использованием SHA-384
    QString passwordHash = SHA384::hashString(password);
    
    // Проверяем авторизацию в базе данных
    bool success = m_db->loginUser(username, passwordHash);
    
    if (success) {
        // Успешная авторизация
        Protocol::Response response(Protocol::ResponseStatus::SUCCESS);
        response.setData("message", "Авторизация успешна");
        response.setData("username", username);
        return response;
    } else {
        // Ошибка авторизации
        Protocol::Response response(Protocol::ResponseStatus::ERROR);
        response.setErrorMessage("Неверное имя пользователя или пароль");
        return response;
    }
} 