#include "chordmethoddialog.h"
#include "ui_chordmethoddialog.h"
#include <QMessageBox>

ChordMethodDialog::ChordMethodDialog(QWidget *parent) : QDialog(parent), ui(new Ui::ChordMethodDialog) {
    ui->setupUi(this);
    
    // Получаем экземпляр синглтона клиента
    m_client = ClientSingleton::getInstance();
    
    // Устанавливаем значения по умолчанию
    ui->equationEdit->setText("1*x^2 -5*x +6");
    ui->aSpinBox->setValue(0);
    ui->bSpinBox->setValue(5);
    ui->epsilonSpinBox->setValue(0.0001);
    ui->maxIterationsSpinBox->setValue(100);
}

ChordMethodDialog::~ChordMethodDialog() {
    delete ui;
}

void ChordMethodDialog::on_solveButton_clicked() {
    QString equation = ui->equationEdit->text();
    double a = ui->aSpinBox->value();
    double b = ui->bSpinBox->value();
    double epsilon = ui->epsilonSpinBox->value();
    int maxIterations = ui->maxIterationsSpinBox->value();
    
    // Проверка на пустое уравнение
    if (equation.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Уравнение не может быть пустым");
        return;
    }
    
    // Проверка на корректность интервала
    if (a >= b) {
        QMessageBox::warning(this, "Ошибка", "Значение a должно быть меньше значения b");
        return;
    }
    
    // Отправляем запрос на решение уравнения
    Protocol::Response response = m_client->solveEquation(equation, a, b, epsilon, maxIterations);
    
    if (response.status() == Protocol::ResponseStatus::SUCCESS) {
        // Решение найдено
        double root = response.data("root").toDouble();
        int iterations = response.data("iterations").toInt();
        double precision = response.data("precision").toDouble();
        
        // Выводим результат
        ui->resultEdit->setText(QString("Корень: %1\nИтераций: %2\nТочность: %3")
                               .arg(root, 0, 'f', 8)
                               .arg(iterations)
                               .arg(precision, 0, 'e', 4));
    } else {
        // Ошибка решения
        ui->resultEdit->setText("Ошибка: " + response.errorMessage());
    }
}

void ChordMethodDialog::on_clearButton_clicked() {
    ui->equationEdit->clear();
    ui->resultEdit->clear();
    ui->aSpinBox->setValue(0);
    ui->bSpinBox->setValue(0);
    ui->epsilonSpinBox->setValue(0.0001);
    ui->maxIterationsSpinBox->setValue(100);
} 