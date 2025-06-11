#ifndef CRYPTODIALOG_H
#define CRYPTODIALOG_H

#include <QDialog>
#include "../client/clientsingleton.h"

namespace Ui {
class CryptoDialog;
}

class CryptoDialog : public QDialog {
    Q_OBJECT
    
public:
    explicit CryptoDialog(QWidget *parent = nullptr);
    ~CryptoDialog();
    
private slots:
    // Обработка нажатия кнопки шифрования
    void on_encryptButton_clicked();
    
    // Обработка нажатия кнопки дешифрования
    void on_decryptButton_clicked();
    
    // Обработка нажатия кнопки генерации ключей
    void on_generateKeysButton_clicked();
    
private:
    Ui::CryptoDialog *ui;
    
    // Ссылка на синглтон клиента
    ClientSingleton *m_client;
    
    // Ключи для Triple DES
    QByteArray m_key1;
    QByteArray m_key2;
    QByteArray m_key3;
    
    // Обновление состояния интерфейса
    void updateUI();
    
    // Генерация случайных ключей
    void generateRandomKeys();
};

#endif // CRYPTODIALOG_H 