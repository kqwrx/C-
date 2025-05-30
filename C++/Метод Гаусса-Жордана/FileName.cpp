#include <iostream>  
#include <omp.h>     

// Функция инициализации матрицы и вектора правых частей
// ar - указатель на массив, n - размер матрицы
void init_ar(double** ar, const int n)
{
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n + 1; j++)
        {
            // Заполняем случайными значениями от 0.0 до 99.9
            ar[i][j] = (double)(rand() % 1000) / 10;
        }
    }
}

// Функция копирования матрицы
// ar1 - исходная матрица, ar2 - целевая матрица, n - размер
void copy(double** ar1, double** ar2, const int n)
{
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n + 1; j++)
        {
            ar2[i][j] = ar1[i][j];  // Поэлементное копирование
        }
    }
}

// Функция вывода матрицы на экран
void print_ar(double** ar, const int n)
{
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n + 1; j++)
        {
            printf("%10.2f", ar[i][j]);  // Форматированный вывод
        }
        printf("\n");  // Переход на новую строку после каждой строки матрицы
    }
    printf("\n");
}

// Функция освобождения памяти
void del(double** ar, const int n)
{
    // Сначала удаляем все вложенные массивы
    for (int i = 0; i < n; i++)
    {
        delete[] ar[i];
    }
    // Затем удаляем основной массив указателей
    delete[] ar;
}

// Функция деления строки матрицы на число
// num - номер строки, k - делитель, par - флаг параллельного выполнения
void divide(double** ar, const int n, int num, double k, bool par)
{
    if (!par)  // Последовательная версия
    {
        for (int i = 0; i < n + 1; i++)
        {
            ar[num][i] /= k;
        }
    }
    else  // Параллельная версия
    {
#pragma omp parallel for  // Директива OpenMP для распараллеливания
        for (int i = 0; i < n + 1; i++)
        {
            ar[num][i] /= k;
        }
    }
}

// Функция вычитания строк (элементарное преобразование строк)
// num1 - вычитаемая строка, num2 - уменьшаемая строка, k - множитель
void subtract(double** ar, const int n, int num1, int num2, double k, bool par)
{
    if (!par)  // Последовательная версия
    {
        for (int i = 0; i < n + 1; i++)
        {
            ar[num2][i] -= k * ar[num1][i];
        }
    }
    else  // Параллельная версия
    {
#pragma omp parallel for
        for (int i = 0; i < n + 1; i++)
        {
            ar[num2][i] -= k * ar[num1][i];
        }
    }
}

// Функция перестановки строк местами
void swap(double** ar, const int n, int num1, int num2, bool par)
{
    if (!par)  // Последовательная версия
    {
        for (int i = 0; i < n + 1; i++)
        {
            double temp = ar[num2][i];
            ar[num2][i] = ar[num1][i];
            ar[num1][i] = temp;
        }
    }
    else  // Параллельная версия
    {
#pragma omp parallel for
        for (int i = 0; i < n + 1; i++)
        {
            double temp = ar[num2][i];
            ar[num2][i] = ar[num1][i];
            ar[num1][i] = temp;
        }
    }
}

// Функция поиска строки с максимальным элементом в столбце
// start - номер столбца, с которого начинаем поиск
int max_el(double** ar, const int n, int start)
{
    double max = 0.0;
    int num = 0;
    for (int i = start; i < n; i++)
    {
        // Ищем максимальный по модулю элемент в столбце start
        if (abs(ar[i][start]) > max)
        {
            max = abs(ar[i][start]);
            num = i;  // Запоминаем номер строки
        }
    }
    return num;  // Возвращаем номер строки с максимальным элементом
}

// Основная функция преобразования матрицы (прямой ход метода Гаусса)
void transform(double** ar, const int n, bool par)
{
    for (int i = 0; i < n; i++)
    {
        // 1. Выбор главного элемента (поиск строки с максимумом в столбце i)
        int num = max_el(ar, n, i);

        // 2. Перестановка строк (если нужно)
        swap(ar, n, i, num, par);

        // 3. Нормировка строки (деление на диагональный элемент)
        divide(ar, n, i, ar[i][i], par);

        // 4. Исключение переменной из всех строк кроме текущей
#pragma omp parallel for  // Параллельный цикл
        for (int j = 0; j < n; j++)
        {
            if (j != i)  // Не обрабатываем текущую строку
            {
                subtract(ar, n, i, j, ar[j][i], par);
            }
        }
    }
}

// Функция решения системы и вывода результата
void solve(double** ar, const int n, bool par)
{
    // Выполняем преобразование матрицы
    transform(ar, n, par);

    // Выводим решение (последний столбец преобразованной матрицы)
    for (int i = 0; i < n; i++)
    {
        printf("%10.2f", ar[i][n]);
    }
}

int main()
{
    setlocale(LC_ALL, "Russian");  // Установка русской локали
    srand(time(0));  // Инициализация генератора случайных чисел

    const int n = 50;  // Размер системы (50 уравнений)

    // Создание и инициализация первой матрицы
    double** ar1 = new double* [n];
    for (int i = 0; i < n; i++)
    {
        ar1[i] = new double[n + 1];  // Матрица коэффициентов + столбец правых частей
    }
    init_ar(ar1, n);  // Заполнение случайными значениями

    // Создание второй матрицы (для параллельного варианта)
    double** ar2 = new double* [n];
    for (int i = 0; i < n; i++)
    {
        ar2[i] = new double[n + 1];
    }
    copy(ar1, ar2, n);  // Копирование данных из первой матрицы

    // Последовательное решение
    double t = omp_get_wtime();  // Замер времени начала
    solve(ar1, n, 0);  // 0 - последовательный режим
    printf("\nПоследовательное время: %f \n", (omp_get_wtime() - t));

    // Параллельное решение
    t = omp_get_wtime();  // Замер времени начала
    solve(ar2, n, 1);  // 1 - параллельный режим
    printf("\nПараллельное время: %f \n", (omp_get_wtime() - t));

    // Освобождение памяти
    del(ar1, n);
    del(ar2, n);

    return EXIT_SUCCESS;
}