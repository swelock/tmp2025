#include "mainwindow.h"
#include "dialogs/authdialog.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    // Создаем и показываем диалог авторизации
    AuthDialog authDialog;
    int result = authDialog.exec();
    
    // Если авторизация успешна, показываем главное окно
    if (result == QDialog::Accepted) {
        MainWindow w;
        w.setUsername(authDialog.username());
        w.show();
        return a.exec();
    }
    
    // Если авторизация не удалась, выходим из приложения
    return 0;
} 