#include <iostream>
#include <vector>

std::vector<double> algorithm(
    const std::vector<double>& a,  // Поддиагональ
    const std::vector<double>& b,  // Главная диагональ
    const std::vector<double>& c,  // Наддиагональ
    const std::vector<double>& d)  // Правая часть
{
    int n = b.size();

    std::vector<double> A(n - 1);    // Прогоночные коэффициенты
    std::vector<double> B(n - 1);
    std::vector<double> x(n);

    // Прямой ход
    A[0] = -c[0] / b[0];
    B[0] = d[0] / b[0];

    for (int i = 1; i < n - 1; ++i)
    {
        double denominator = a[i] * A[i - 1] + b[i];
        A[i] = -c[i] / denominator; // по формуле
        B[i] = (d[i] - a[i] * B[i - 1]) / denominator; // по формуле
    }

    // Обработка последнего уравнения
    double denominator = a[n - 1] * A[n - 2] + b[n - 1];
    x[n - 1] = (d[n - 1] - a[n - 1] * B[n - 2]) / denominator;

    // Обратный ход
    for (int i = n - 2; i >= 0; --i)
    {
        x[i] = A[i] * x[i + 1] + B[i];
    }

    return x;
}

int main()
{
    // Пример системы:
    // 2x0 + x1         = 1
    // x0 + 2x1 + x2    = 2
    //      x1 + 2x2    = 3
    //решение: x0=0.5, x1=0, x2=1.5

    std::vector<double> a = { 0.0, 1.0, 1.0 };
    std::vector<double> b = { 2.0, 2.0, 2.0 };
    std::vector<double> c = { 1.0, 1.0, 0.0 };
    std::vector<double> d = { 1.0, 2.0, 3.0 };

    std::vector<double> x = algorithm(a, b, c, d);

    std::cout << "answer: ";
    for (double xi : x)
    {
        std::cout << xi << " ";
    }
    std::cout << std::endl;

    return 0;
}