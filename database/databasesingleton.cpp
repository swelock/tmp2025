#include "databasesingleton.h"
#include "../../common/crypto/sha384.h"

// Инициализация статических членов
DatabaseSingleton* DatabaseSingleton::instance = nullptr;
QMutex DatabaseSingleton::mutex;

DatabaseSingleton::DatabaseSingleton(QObject *parent) : QObject(parent) {
    // Пустой конструктор
}

DatabaseSingleton* DatabaseSingleton::getInstance() {
    // Потокобезопасное создание синглтона
    QMutexLocker locker(&mutex);
    
    if (!instance) {
        instance = new DatabaseSingleton();
    }
    
    return instance;
}

// Деструктор для освобождения ресурсов
DatabaseSingleton::~DatabaseSingleton() {
    close();
}

bool DatabaseSingleton::init(const QString &dbPath) {
    // Проверяем, не открыта ли уже база данных
    if (m_db.isOpen()) {
        return true;
    }
    
    // Открываем соединение с базой данных
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(dbPath);
    
    if (!m_db.open()) {
        m_lastError = m_db.lastError().text();
        qDebug() << "Ошибка открытия базы данных:" << m_lastError;
        return false;
    }
    
    // Создаем таблицы, если их нет
    if (!createTables()) {
        return false;
    }
    
    qDebug() << "База данных успешно инициализирована";
    return true;
}

void DatabaseSingleton::close() {
    if (m_db.isOpen()) {
        m_db.close();
    }
    
    // Удаляем соединение с базой данных
    QString connectionName = m_db.connectionName();
    m_db = QSqlDatabase(); // Закрываем соединение
    QSqlDatabase::removeDatabase(connectionName); // Удаляем соединение из списка
}

bool DatabaseSingleton::createTables() {
    QSqlQuery query;
    
    // Создаем таблицу пользователей
    if (!query.exec("CREATE TABLE IF NOT EXISTS users ("
                   "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                   "username TEXT UNIQUE NOT NULL, "
                   "password_hash TEXT NOT NULL, "
                   "created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP)")) {
        m_lastError = query.lastError().text();
        qDebug() << "Ошибка создания таблицы users:" << m_lastError;
        return false;
    }
    
    return true;
}

bool DatabaseSingleton::registerUser(const QString &username, const QString &passwordHash) {
    if (userExists(username)) {
        m_lastError = "Пользователь с таким именем уже существует";
        return false;
    }
    
    QSqlQuery query;
    query.prepare("INSERT INTO users (username, password_hash) VALUES (:username, :password_hash)");
    query.bindValue(":username", username);
    query.bindValue(":password_hash", passwordHash);
    
    if (!query.exec()) {
        m_lastError = query.lastError().text();
        qDebug() << "Ошибка регистрации пользователя:" << m_lastError;
        return false;
    }
    
    return true;
}

bool DatabaseSingleton::loginUser(const QString &username, const QString &passwordHash) {
    QSqlQuery query;
    query.prepare("SELECT id FROM users WHERE username = :username AND password_hash = :password_hash");
    query.bindValue(":username", username);
    query.bindValue(":password_hash", passwordHash);
    
    if (!query.exec()) {
        m_lastError = query.lastError().text();
        qDebug() << "Ошибка авторизации пользователя:" << m_lastError;
        return false;
    }
    
    // Если найдена запись, значит авторизация успешна
    return query.next();
}

bool DatabaseSingleton::userExists(const QString &username) {
    QSqlQuery query;
    query.prepare("SELECT id FROM users WHERE username = :username");
    query.bindValue(":username", username);
    
    if (!query.exec()) {
        m_lastError = query.lastError().text();
        qDebug() << "Ошибка проверки существования пользователя:" << m_lastError;
        return false;
    }
    
    return query.next();
}

QString DatabaseSingleton::lastError() const {
    return m_lastError;
} 