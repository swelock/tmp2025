#ifndef STEGANOGRAPHYDIALOG_H
#define STEGANOGRAPHYDIALOG_H

#include <QDialog>
#include <QImage>
#include "../client/clientsingleton.h"

namespace Ui {
class SteganographyDialog;
}

class SteganographyDialog : public QDialog {
    Q_OBJECT
    
public:
    explicit SteganographyDialog(QWidget *parent = nullptr);
    ~SteganographyDialog();
    
private slots:
    // Обработка нажатия кнопки загрузки изображения
    void on_buttonLoadImage_clicked();
    
    // Обработка нажатия кнопки скрытия сообщения
    void on_buttonHideMessage_clicked();
    
    // Обработка нажатия кнопки извлечения сообщения
    void on_buttonExtractMessage_clicked();
    
    // Обработка нажатия кнопки сохранения изображения
    void on_buttonSaveResult_clicked();
    
    // Обработка нажатия кнопки загрузки изображения для извлечения
    void on_buttonLoadExtractImage_clicked();
    
private:
    Ui::SteganographyDialog *ui;
    
    // Ссылка на синглтон клиента
    ClientSingleton *m_client;
    
    // Исходное изображение
    QImage m_sourceImage;
    
    // Изображение с внедренным сообщением
    QImage m_stegoImage;
    
    // Обновление состояния интерфейса
    void updateUI();
    
    // Отображение изображения в интерфейсе
    void displayImage(const QImage &image);
};

#endif // STEGANOGRAPHYDIALOG_H 