#include "authdialog.h"
#include "ui_authdialog.h"

AuthDialog::AuthDialog(QWidget *parent) : QDialog(parent), ui(new Ui::AuthDialog) {
    ui->setupUi(this);
    
    // Получаем экземпляр синглтона клиента
    m_client = ClientSingleton::getInstance();
    
    // Подключаем сигнал изменения состояния подключения
    connect(m_client, &ClientSingleton::connectionStateChanged, this, &AuthDialog::onConnectionStateChanged);
    
    // Устанавливаем начальное состояние интерфейса
    updateUI();
    
    // Устанавливаем фокус на поле хоста
    ui->hostEdit->setFocus();
}

AuthDialog::~AuthDialog() {
    delete ui;
}

QString AuthDialog::username() const {
    return ui->usernameEdit->text();
}

void AuthDialog::on_loginButton_clicked() {
    QString username = ui->usernameEdit->text();
    QString password = ui->passwordEdit->text();
    
    // Проверка на пустые поля
    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Имя пользователя и пароль не могут быть пустыми");
        return;
    }
    
    // Отправляем запрос на авторизацию
    Protocol::Response response = m_client->login(username, password);
    
    if (response.status() == Protocol::ResponseStatus::SUCCESS) {
        // Авторизация успешна
        accept();
    } else {
        // Ошибка авторизации
        QMessageBox::critical(this, "Ошибка авторизации", response.errorMessage());
    }
}

void AuthDialog::on_registerButton_clicked() {
    QString username = ui->usernameEdit->text();
    QString password = ui->passwordEdit->text();
    
    // Проверка на пустые поля
    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Имя пользователя и пароль не могут быть пустыми");
        return;
    }
    
    // Отправляем запрос на регистрацию
    Protocol::Response response = m_client->registerUser(username, password);
    
    if (response.status() == Protocol::ResponseStatus::SUCCESS) {
        // Регистрация успешна
        QMessageBox::information(this, "Регистрация", "Регистрация успешно завершена. Теперь вы можете войти.");
    } else {
        // Ошибка регистрации
        QMessageBox::critical(this, "Ошибка регистрации", response.errorMessage());
    }
}

void AuthDialog::on_connectButton_clicked() {
    QString host = ui->hostEdit->text();
    int port = ui->portSpinBox->value();
    
    // Проверка на пустое поле хоста
    if (host.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Хост не может быть пустым");
        return;
    }
    
    // Если уже подключены, отключаемся
    if (m_client->isConnected()) {
        m_client->disconnectFromServer();
        updateUI();
        return;
    }
    
    // Подключаемся к серверу
    bool connected = m_client->connectToServer(host, port);
    
    if (!connected) {
        QMessageBox::critical(this, "Ошибка подключения", "Не удалось подключиться к серверу");
    }
    
    updateUI();
}

void AuthDialog::onConnectionStateChanged(bool connected) {
    Q_UNUSED(connected);
    updateUI();
}

void AuthDialog::updateUI() {
    bool connected = m_client->isConnected();
    
    // Обновляем текст кнопки подключения
    ui->connectButton->setText(connected ? "Отключиться" : "Подключиться");
    
    // Включаем/отключаем поля ввода хоста и порта
    ui->hostEdit->setEnabled(!connected);
    ui->portSpinBox->setEnabled(!connected);
    
    // Включаем/отключаем кнопки авторизации и регистрации
    ui->loginButton->setEnabled(connected);
    ui->registerButton->setEnabled(connected);
    
    // Обновляем статус подключения
    ui->statusLabel->setText(connected ? "Подключено" : "Не подключено");
    ui->statusLabel->setStyleSheet(connected ? "color: green;" : "color: red;");
} 