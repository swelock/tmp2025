#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dialogs/authdialog.h"
#include "dialogs/cryptodialog.h"
#include "dialogs/chordmethoddialog.h"
#include "dialogs/steganographydialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    
    // Устанавливаем заголовок окна
    setWindowTitle(tr("Ильин Кирилл Александрович 241-353"));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setUsername(const QString &username)
{
    m_username = username;
    // Обновляем заголовок окна, добавляя имя пользователя
    setWindowTitle(tr("Ильин Кирилл Александрович 241-353 - Пользователь: %1").arg(username));
}

void MainWindow::on_actionCrypto_triggered()
{
    CryptoDialog dialog(this);
    dialog.exec();
}

void MainWindow::on_actionChordMethod_triggered()
{
    ChordMethodDialog dialog(this);
    dialog.exec();
}

void MainWindow::on_actionSteganography_triggered()
{
    SteganographyDialog dialog(this);
    dialog.exec();
}

void MainWindow::on_cryptoButton_clicked()
{
    on_actionCrypto_triggered();
}

void MainWindow::on_chordMethodButton_clicked()
{
    on_actionChordMethod_triggered();
}

void MainWindow::on_steganographyButton_clicked()
{
    on_actionSteganography_triggered();
} 