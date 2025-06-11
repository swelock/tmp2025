#ifndef AUTHDIALOG_H
#define AUTHDIALOG_H

#include <QDialog>
#include <QMessageBox>
#include "../client/clientsingleton.h"

namespace Ui {
class AuthDialog;
}

class AuthDialog : public QDialog {
    Q_OBJECT
    
public:
    explicit AuthDialog(QWidget *parent = nullptr);
    ~AuthDialog();
    
    // Получение имени пользователя
    QString username() const;
    
private slots:
    // Обработка нажатия кнопки входа
    void on_loginButton_clicked();
    
    // Обработка нажатия кнопки регистрации
    void on_registerButton_clicked();
    
    // Обработка нажатия кнопки подключения к серверу
    void on_connectButton_clicked();
    
    // Обработка изменения состояния подключения
    void onConnectionStateChanged(bool connected);
    
private:
    Ui::AuthDialog *ui;
    
    // Ссылка на синглтон клиента
    ClientSingleton *m_client;
    
    // Обновление состояния интерфейса
    void updateUI();
};

#endif // AUTHDIALOG_H 