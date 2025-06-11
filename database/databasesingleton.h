#ifndef DATABASESINGLETON_H
#define DATABASESINGLETON_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QMutex>
#include <QDebug>

class DatabaseSingleton : public QObject {
    Q_OBJECT
    
public:
    // Получение экземпляра синглтона
    static DatabaseSingleton* getInstance();
    
    // Инициализация базы данных
    bool init(const QString &dbPath = "server.db");
    
    // Закрытие соединения с базой данных
    void close();
    
    // Методы для работы с пользователями
    bool registerUser(const QString &username, const QString &passwordHash);
    bool loginUser(const QString &username, const QString &passwordHash);
    bool userExists(const QString &username);
    
    // Получение ошибки
    QString lastError() const;
    
private:
    // Приватный конструктор (синглтон)
    explicit DatabaseSingleton(QObject *parent = nullptr);
    
    // Деструктор
    ~DatabaseSingleton();
    
    // Запрещаем копирование и присваивание
    DatabaseSingleton(const DatabaseSingleton&) = delete;
    DatabaseSingleton& operator=(const DatabaseSingleton&) = delete;
    
    // Создание таблиц базы данных
    bool createTables();
    
    // Статический экземпляр синглтона
    static DatabaseSingleton* instance;
    
    // Мьютекс для потокобезопасности
    static QMutex mutex;
    
    // База данных
    QSqlDatabase m_db;
    
    // Последняя ошибка
    QString m_lastError;
};

#endif // DATABASESINGLETON_H 