#include "cryptodialog.h"
#include "ui_cryptodialog.h"
#include <QMessageBox>
#include "../../common/crypto/tripledes.h"

CryptoDialog::CryptoDialog(QWidget *parent) : QDialog(parent), ui(new Ui::CryptoDialog) {
    ui->setupUi(this);
    
    // Получаем экземпляр синглтона клиента
    m_client = ClientSingleton::getInstance();
    
    // Генерируем начальные ключи
    generateRandomKeys();
    
    // Устанавливаем начальное состояние интерфейса
    updateUI();
}

CryptoDialog::~CryptoDialog() {
    delete ui;
}

void CryptoDialog::on_encryptButton_clicked() {
    QString text = ui->plainTextEdit->toPlainText();
    
    // Проверка на пустой текст
    if (text.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Текст для шифрования не может быть пустым");
        return;
    }
    
    // Отправляем запрос на шифрование
    Protocol::Response response = m_client->encryptTripleDES(text, m_key1, m_key2, m_key3);
    
    if (response.status() == Protocol::ResponseStatus::SUCCESS) {
        // Шифрование успешно
        QString encryptedText = response.data("encrypted_text").toString();
        ui->cipherTextEdit->setPlainText(encryptedText);
    } else {
        // Ошибка шифрования
        QMessageBox::critical(this, "Ошибка шифрования", response.errorMessage());
    }
}

void CryptoDialog::on_decryptButton_clicked() {
    QString encryptedText = ui->cipherTextEdit->toPlainText();
    
    // Проверка на пустой текст
    if (encryptedText.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Зашифрованный текст не может быть пустым");
        return;
    }
    
    // Отправляем запрос на дешифрование
    Protocol::Response response = m_client->decryptTripleDES(encryptedText, m_key1, m_key2, m_key3);
    
    if (response.status() == Protocol::ResponseStatus::SUCCESS) {
        // Дешифрование успешно
        QString decryptedText = response.data("decrypted_text").toString();
        ui->plainTextEdit->setPlainText(decryptedText);
    } else {
        // Ошибка дешифрования
        QMessageBox::critical(this, "Ошибка дешифрования", response.errorMessage());
    }
}

void CryptoDialog::on_generateKeysButton_clicked() {
    generateRandomKeys();
    updateUI();
}

void CryptoDialog::generateRandomKeys() {
    // Генерируем случайные ключи для Triple DES
    m_key1 = TripleDES::generateRandomKey();
    m_key2 = TripleDES::generateRandomKey();
    m_key3 = TripleDES::generateRandomKey();
}

void CryptoDialog::updateUI() {
    // Отображаем ключи в интерфейсе
    ui->key1Edit->setText(m_key1.toBase64());
    ui->key2Edit->setText(m_key2.toBase64());
    ui->key3Edit->setText(m_key3.toBase64());
} 