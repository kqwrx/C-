#include <iostream>  
#include <omp.h>     

// ������� ������������� ������� � ������� ������ ������
// ar - ��������� �� ������, n - ������ �������
void init_ar(double** ar, const int n)
{
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n + 1; j++)
        {
            // ��������� ���������� ���������� �� 0.0 �� 99.9
            ar[i][j] = (double)(rand() % 1000) / 10;
        }
    }
}

// ������� ����������� �������
// ar1 - �������� �������, ar2 - ������� �������, n - ������
void copy(double** ar1, double** ar2, const int n)
{
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n + 1; j++)
        {
            ar2[i][j] = ar1[i][j];  // ������������ �����������
        }
    }
}

// ������� ������ ������� �� �����
void print_ar(double** ar, const int n)
{
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n + 1; j++)
        {
            printf("%10.2f", ar[i][j]);  // ��������������� �����
        }
        printf("\n");  // ������� �� ����� ������ ����� ������ ������ �������
    }
    printf("\n");
}

// ������� ������������ ������
void del(double** ar, const int n)
{
    // ������� ������� ��� ��������� �������
    for (int i = 0; i < n; i++)
    {
        delete[] ar[i];
    }
    // ����� ������� �������� ������ ����������
    delete[] ar;
}

// ������� ������� ������ ������� �� �����
// num - ����� ������, k - ��������, par - ���� ������������� ����������
void divide(double** ar, const int n, int num, double k, bool par)
{
    if (!par)  // ���������������� ������
    {
        for (int i = 0; i < n + 1; i++)
        {
            ar[num][i] /= k;
        }
    }
    else  // ������������ ������
    {
#pragma omp parallel for  // ��������� OpenMP ��� �����������������
        for (int i = 0; i < n + 1; i++)
        {
            ar[num][i] /= k;
        }
    }
}

// ������� ��������� ����� (������������ �������������� �����)
// num1 - ���������� ������, num2 - ����������� ������, k - ���������
void subtract(double** ar, const int n, int num1, int num2, double k, bool par)
{
    if (!par)  // ���������������� ������
    {
        for (int i = 0; i < n + 1; i++)
        {
            ar[num2][i] -= k * ar[num1][i];
        }
    }
    else  // ������������ ������
    {
#pragma omp parallel for
        for (int i = 0; i < n + 1; i++)
        {
            ar[num2][i] -= k * ar[num1][i];
        }
    }
}

// ������� ������������ ����� �������
void swap(double** ar, const int n, int num1, int num2, bool par)
{
    if (!par)  // ���������������� ������
    {
        for (int i = 0; i < n + 1; i++)
        {
            double temp = ar[num2][i];
            ar[num2][i] = ar[num1][i];
            ar[num1][i] = temp;
        }
    }
    else  // ������������ ������
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

// ������� ������ ������ � ������������ ��������� � �������
// start - ����� �������, � �������� �������� �����
int max_el(double** ar, const int n, int start)
{
    double max = 0.0;
    int num = 0;
    for (int i = start; i < n; i++)
    {
        // ���� ������������ �� ������ ������� � ������� start
        if (abs(ar[i][start]) > max)
        {
            max = abs(ar[i][start]);
            num = i;  // ���������� ����� ������
        }
    }
    return num;  // ���������� ����� ������ � ������������ ���������
}

// �������� ������� �������������� ������� (������ ��� ������ ������)
void transform(double** ar, const int n, bool par)
{
    for (int i = 0; i < n; i++)
    {
        // 1. ����� �������� �������� (����� ������ � ���������� � ������� i)
        int num = max_el(ar, n, i);

        // 2. ������������ ����� (���� �����)
        swap(ar, n, i, num, par);

        // 3. ���������� ������ (������� �� ������������ �������)
        divide(ar, n, i, ar[i][i], par);

        // 4. ���������� ���������� �� ���� ����� ����� �������
#pragma omp parallel for  // ������������ ����
        for (int j = 0; j < n; j++)
        {
            if (j != i)  // �� ������������ ������� ������
            {
                subtract(ar, n, i, j, ar[j][i], par);
            }
        }
    }
}

// ������� ������� ������� � ������ ����������
void solve(double** ar, const int n, bool par)
{
    // ��������� �������������� �������
    transform(ar, n, par);

    // ������� ������� (��������� ������� ��������������� �������)
    for (int i = 0; i < n; i++)
    {
        printf("%10.2f", ar[i][n]);
    }
}

int main()
{
    setlocale(LC_ALL, "Russian");  // ��������� ������� ������
    srand(time(0));  // ������������� ���������� ��������� �����

    const int n = 50;  // ������ ������� (50 ���������)

    // �������� � ������������� ������ �������
    double** ar1 = new double* [n];
    for (int i = 0; i < n; i++)
    {
        ar1[i] = new double[n + 1];  // ������� ������������� + ������� ������ ������
    }
    init_ar(ar1, n);  // ���������� ���������� ����������

    // �������� ������ ������� (��� ������������� ��������)
    double** ar2 = new double* [n];
    for (int i = 0; i < n; i++)
    {
        ar2[i] = new double[n + 1];
    }
    copy(ar1, ar2, n);  // ����������� ������ �� ������ �������

    // ���������������� �������
    double t = omp_get_wtime();  // ����� ������� ������
    solve(ar1, n, 0);  // 0 - ���������������� �����
    printf("\n���������������� �����: %f \n", (omp_get_wtime() - t));

    // ������������ �������
    t = omp_get_wtime();  // ����� ������� ������
    solve(ar2, n, 1);  // 1 - ������������ �����
    printf("\n������������ �����: %f \n", (omp_get_wtime() - t));

    // ������������ ������
    del(ar1, n);
    del(ar2, n);

    return EXIT_SUCCESS;
}