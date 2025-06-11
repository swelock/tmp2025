#ifndef CHORDMETHODDIALOG_H
#define CHORDMETHODDIALOG_H

#include <QDialog>
#include "../client/clientsingleton.h"

namespace Ui {
class ChordMethodDialog;
}

class ChordMethodDialog : public QDialog {
    Q_OBJECT
    
public:
    explicit ChordMethodDialog(QWidget *parent = nullptr);
    ~ChordMethodDialog();
    
private slots:
    // Обработка нажатия кнопки решения
    void on_solveButton_clicked();
    
    // Обработка нажатия кнопки очистки
    void on_clearButton_clicked();
    
private:
    Ui::ChordMethodDialog *ui;
    
    // Ссылка на синглтон клиента
    ClientSingleton *m_client;
};

#endif // CHORDMETHODDIALOG_H 