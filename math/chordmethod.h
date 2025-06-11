#ifndef CHORDMETHOD_H
#define CHORDMETHOD_H

#include <QString>
#include <QVector>
#include <functional>

class ChordMethod {
public:
    ChordMethod();
    
    // Структура для результата решения
    struct Result {
        bool success;
        double root;
        int iterations;
        double precision;
        QString errorMessage;
    };
    
    // Решает уравнение методом хорд
    static Result solveEquation(const QString &equation, double a, double b, double epsilon = 0.0001, int maxIterations = 100);
    
    // Решает уравнение методом хорд с использованием функции
    static Result solve(const std::function<double(double)> &func, double a, double b, double epsilon = 0.0001, int maxIterations = 100);
    
private:
    // Парсит строку уравнения и возвращает функцию для вычисления
    static std::function<double(double)> parseEquation(const QString &equation, bool &success, QString &errorMessage);
};

#endif // CHORDMETHOD_H 