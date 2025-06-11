#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    
    // Метод для установки имени пользователя
    void setUsername(const QString &username);

private slots:
    void on_actionCrypto_triggered();
    void on_actionChordMethod_triggered();
    void on_actionSteganography_triggered();
    
    // Слоты для кнопок
    void on_cryptoButton_clicked();
    void on_chordMethodButton_clicked();
    void on_steganographyButton_clicked();

private:
    Ui::MainWindow *ui;
    QString m_username; // Хранение имени пользователя
};
#endif // MAINWINDOW_H 