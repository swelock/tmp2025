#include <QCoreApplication>
#include <QCommandLineParser>
#include <QTimer>
#include <QDebug>
#include "server.h"

// Глобальный указатель на сервер для доступа из функции
Server* g_server = nullptr;

// Функция для вывода количества подключенных клиентов
void printClientCount() {
    if (g_server && g_server->isRunning()) {
        qDebug() << "Количество подключенных клиентов:" << g_server->getClientCount();
    }
}

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("QtClientServer-Server");
    QCoreApplication::setApplicationVersion("1.0");
    
    // Парсер аргументов командной строки
    QCommandLineParser parser;
    parser.setApplicationDescription("Сервер для клиент-серверного приложения на Qt");
    parser.addHelpOption();
    parser.addVersionOption();
    
    // Опция для порта
    QCommandLineOption portOption(QStringList() << "p" << "port",
                                 "Порт для прослушивания (по умолчанию 12345).",
                                 "port", "12345");
    parser.addOption(portOption);
    
    // Обрабатываем аргументы
    parser.process(app);
    
    // Получаем порт
    int port = parser.value(portOption).toInt();
    
    // Создаем и запускаем сервер
    Server server;
    g_server = &server;
    if (!server.start(port)) {
        return 1;
    }
    
    // Создаем таймер для периодического вывода количества клиентов
    QTimer clientCountTimer;
    QObject::connect(&clientCountTimer, &QTimer::timeout, printClientCount);
    clientCountTimer.start(5000); // Вывод каждые 5 секунд
    
    return app.exec();
} 