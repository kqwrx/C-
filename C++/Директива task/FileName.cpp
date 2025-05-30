#include <iostream>
#include <cmath>
#include <omp.h>

using namespace std;

// 1. Функция вычисления подынтегрального выражения
double f(double x, double alpha, double beta) {
    // Вычисление первого квадратного корня
    double term1 = sqrt(1 - 2 * alpha * x + alpha * alpha);
    // Вычисление второго квадратного корня
    double term2 = sqrt(1 - 2 * beta * x + beta * beta);
    // Возвращаем значение функции
    return 1.0 / (term1 * term2);
}

// 2. Функция численного интегрирования методом трапеций
double integr(double a, double b, double (*func)(double, double, double),
    double alpha, double beta, int n = 10) {
    // Вычисление шага интегрирования
    double h = (b - a) / n;
    // Начальное значение суммы - полусумма значений на концах отрезка
    double sum = 0.5 * (func(a, alpha, beta) + func(b, alpha, beta));

    // Суммирование значений функции в промежуточных точках
    for (int i = 1; i < n; ++i) {
        double x = a + i * h;  // Текущая точка на отрезке
        sum += func(x, alpha, beta);  // Добавляем значение функции
    }

    // Умножаем сумму на шаг для получения интеграла
    return sum * h;
}

// 3. Адаптивная квадратура (последовательная рекурсивная версия)
double integral_serial(double a, double b, double (*func)(double, double, double),
    double alpha, double beta) {
    // Вычисление интеграла на всем отрезке
    double whole = integr(a, b, func, alpha, beta);
    // Находим середину отрезка
    double c = (a + b) / 2;
    // Вычисляем интегралы на левой и правой половинах
    double left = integr(a, c, func, alpha, beta);
    double right = integr(c, b, func, alpha, beta);

    // Проверка точности (разница между целым и суммой половин)
    if (fabs(whole - (left + right)) <= 1e-10) {
        // Если точность достигнута - возвращаем сумму
        return left + right;
    }
    else {
        // Иначе - рекурсивно уточняем на каждой половине
        return integral_serial(a, c, func, alpha, beta) +
            integral_serial(c, b, func, alpha, beta);
    }
}

// 4. Адаптивная квадратура (параллельная версия с OpenMP)
double integral_parallel(double a, double b, double (*func)(double, double, double),
    double alpha, double beta) {
    // Вычисление интеграла на всем отрезке
    double whole = integr(a, b, func, alpha, beta);
    // Находим середину отрезка
    double c = (a + b) / 2;
    double left, right;  // Переменные для хранения результатов половин

    // Создаем задачу для левой половины (выполнится параллельно)
#pragma omp task shared(left)
    left = integr(a, c, func, alpha, beta);

    // Создаем задачу для правой половины (выполнится параллельно)
#pragma omp task shared(right)
    right = integr(c, b, func, alpha, beta);

    // Ожидаем завершения обеих задач
#pragma omp taskwait

// Проверка точности
    if (fabs(whole - (left + right)) <= 1e-10) {
        // Если точность достигнута - возвращаем сумму
        return left + right;
    }
    else {
        double res_left, res_right;  // Результаты рекурсивных вызовов

        // Рекурсивный вызов для левой половины (параллельно)
#pragma omp task shared(res_left)
        res_left = integral_parallel(a, c, func, alpha, beta);

        // Рекурсивный вызов для правой половины (параллельно)
#pragma omp task shared(res_right)
        res_right = integral_parallel(c, b, func, alpha, beta);

        // Ожидаем завершения рекурсивных задач
#pragma omp taskwait

// Возвращаем сумму результатов
        return res_left + res_right;
    }
}

int main() {
    // Параметры задачи
    double alpha = 0.5;  // Параметр α
    double beta = 0.5;   // Параметр β
    double a = 0.0;      // Нижний предел интегрирования
    double b = 1.0;      // Верхний предел интегрирования

    // Последовательное вычисление
    cout << "Serial adaptive quadrature:" << endl;
    double serial_result = integral_serial(a, b, f, alpha, beta);
    cout << "Result: " << serial_result << endl;

    // Параллельное вычисление с OpenMP
    cout << "\nParallel adaptive quadrature with OpenMP:" << endl;
    // Создаем параллельную область
#pragma omp parallel
    {
        // Только один поток выполняет этот блок
#pragma omp single
        {
            // Запускаем параллельное вычисление
            double parallel_result = integral_parallel(a, b, f, alpha, beta);
            cout << "Result: " << parallel_result << endl;
        }
    }

    return 0;  // Завершение программы
}