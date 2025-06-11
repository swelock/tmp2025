#include "steganographydialog.h"
#include "ui_steganographydialog.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QPixmap>

SteganographyDialog::SteganographyDialog(QWidget *parent) : QDialog(parent), ui(new Ui::SteganographyDialog) {
    ui->setupUi(this);
    
    // Получаем экземпляр синглтона клиента
    m_client = ClientSingleton::getInstance();
    
    // Устанавливаем начальное состояние интерфейса
    updateUI();
}

SteganographyDialog::~SteganographyDialog() {
    delete ui;
}

// Обработчик кнопки загрузки изображения
void SteganographyDialog::on_buttonLoadImage_clicked() {
    // Открываем диалог выбора файла
    QString filePath = QFileDialog::getOpenFileName(this, "Открыть изображение", "", "Изображения (*.png *.jpg *.bmp)");
    
    if (filePath.isEmpty()) {
        return;
    }
    
    // Загружаем изображение
    m_sourceImage.load(filePath);
    
    if (m_sourceImage.isNull()) {
        QMessageBox::critical(this, "Ошибка", "Не удалось загрузить изображение");
        return;
    }
    
    // Отображаем изображение
    displayImage(m_sourceImage);
    
    // Очищаем поля
    ui->textEditMessage->clear();
    ui->textEditExtractedMessage->clear();
    
    // Обновляем интерфейс
    updateUI();
}

// Обработчик кнопки скрытия сообщения
void SteganographyDialog::on_buttonHideMessage_clicked() {
    // Проверяем, что изображение загружено
    if (m_sourceImage.isNull()) {
        QMessageBox::warning(this, "Ошибка", "Сначала загрузите изображение");
        return;
    }
    
    // Получаем сообщение для скрытия
    QString message = ui->textEditMessage->toPlainText();
    
    if (message.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите сообщение для скрытия");
        return;
    }
    
    // Отправляем запрос на скрытие сообщения
    Protocol::Response response = m_client->hideMessage(m_sourceImage, message);
    
    if (response.status() == Protocol::ResponseStatus::SUCCESS) {
        // Получаем изображение с внедренным сообщением
        QByteArray imageData = QByteArray::fromBase64(response.data("image").toString().toUtf8());
        m_stegoImage.loadFromData(imageData);
        
        // Отображаем изображение с внедренным сообщением
        displayImage(m_stegoImage);
        
        // Выводим сообщение об успехе
        QMessageBox::information(this, "Успех", "Сообщение успешно скрыто в изображении");
    } else {
        // Выводим сообщение об ошибке
        QMessageBox::critical(this, "Ошибка", response.errorMessage());
    }
    
    // Обновляем интерфейс
    updateUI();
}

// Обработчик кнопки извлечения сообщения
void SteganographyDialog::on_buttonExtractMessage_clicked() {
    // Проверяем, что изображение загружено
    if (m_sourceImage.isNull()) {
        QMessageBox::warning(this, "Ошибка", "Сначала загрузите изображение");
        return;
    }
    
    // Используем текущее отображаемое изображение
    QImage image = m_stegoImage.isNull() ? m_sourceImage : m_stegoImage;
    
    // Отправляем запрос на извлечение сообщения
    Protocol::Response response = m_client->extractMessage(image);
    
    if (response.status() == Protocol::ResponseStatus::SUCCESS) {
        // Получаем извлеченное сообщение
        QString message = response.data("message").toString();
        
        // Отображаем извлеченное сообщение
        ui->textEditExtractedMessage->setPlainText(message);
    } else {
        // Выводим сообщение об ошибке
        QMessageBox::critical(this, "Ошибка", response.errorMessage());
    }
}

// Обработчик кнопки сохранения изображения
void SteganographyDialog::on_buttonSaveResult_clicked() {
    // Проверяем, что изображение с внедренным сообщением существует
    if (m_stegoImage.isNull()) {
        QMessageBox::warning(this, "Ошибка", "Сначала скройте сообщение в изображении");
        return;
    }
    
    // Открываем диалог сохранения файла
    QString filePath = QFileDialog::getSaveFileName(this, "Сохранить изображение", "", "PNG (*.png)");
    
    if (filePath.isEmpty()) {
        return;
    }
    
    // Сохраняем изображение
    if (!m_stegoImage.save(filePath, "PNG")) {
        QMessageBox::critical(this, "Ошибка", "Не удалось сохранить изображение");
        return;
    }
    
    QMessageBox::information(this, "Успех", "Изображение успешно сохранено");
}

// Обработчик кнопки загрузки изображения для извлечения
void SteganographyDialog::on_buttonLoadExtractImage_clicked() {
    // Открываем диалог выбора файла
    QString filePath = QFileDialog::getOpenFileName(this, "Открыть изображение", "", "Изображения (*.png *.jpg *.bmp)");
    
    if (filePath.isEmpty()) {
        return;
    }
    
    // Загружаем изображение
    m_sourceImage.load(filePath);
    
    if (m_sourceImage.isNull()) {
        QMessageBox::critical(this, "Ошибка", "Не удалось загрузить изображение");
        return;
    }
    
    // Отображаем изображение в соответствующей метке
    QPixmap pixmap = QPixmap::fromImage(m_sourceImage).scaled(ui->labelExtractImage->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->labelExtractImage->setPixmap(pixmap);
    
    // Обновляем интерфейс
    updateUI();
}

void SteganographyDialog::updateUI() {
    // Включаем/отключаем кнопки в зависимости от наличия изображения
    bool hasImage = !m_sourceImage.isNull();
    bool hasStegoImage = !m_stegoImage.isNull();
    
    ui->buttonHideMessage->setEnabled(hasImage);
    ui->buttonExtractMessage->setEnabled(hasImage);
    ui->buttonSaveResult->setEnabled(hasStegoImage);
}

void SteganographyDialog::displayImage(const QImage &image) {
    // Масштабируем изображение до размера метки
    QPixmap pixmap = QPixmap::fromImage(image).scaled(ui->labelSourceImage->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    
    // Отображаем изображение в соответствующей метке на вкладке скрытия
    ui->labelSourceImage->setPixmap(pixmap);
    
    // Также отображаем результат, если это стегоизображение
    if (!m_stegoImage.isNull() && image == m_stegoImage) {
        QPixmap resultPixmap = pixmap;
        ui->labelResultImage->setPixmap(resultPixmap);
    }
} 