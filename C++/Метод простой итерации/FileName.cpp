#include <iostream>
#include <vector>
#include <cmath>
#include <omp.h> 
#include <chrono>

using namespace std;

const double C = 1e-6;

void convertToHForm(const vector<vector<double>>& A, const vector<double>& b,
    vector<vector<double>>& H, vector<double>& g)
{
    int n = A.size();

    H.resize(n, vector<double>(n, 0.0));
    g.resize(n, 0.0);

    for (int i = 0; i < n; ++i)
    {
        double a_ii = A[i][i];
        if (a_ii == 0)
        {
            cerr << "Zero diagonal in row " << i << endl;
            exit(1);
        }

        g[i] = b[i] / a_ii;

        for (int j = 0; j < n; ++j)
        {
            H[i][j] = (i != j) ? -A[i][j] / a_ii : 0.0;
        }
    }
}

double computeHNorm(const vector<vector<double>>& H)
{
    double norm = 0.0;
    for (int i = 0; i < H.size(); ++i)
    {
        double sum = 0;
        for (int j = 0; j < H[i].size(); ++j)
        {
            sum += abs(H[i][j]);
        }
        if (sum > norm)
        {
            norm = sum;
        }
    }
    return norm;
}

void iterate(const vector<vector<double>>& H, const vector<double>& g,
    const vector<double>& x, vector<double>& x_new) //новое приближение
{
    int n = H.size();

#pragma omp parallel for
    for (int i = 0; i < n; ++i)
    {
        x_new[i] = g[i];
        for (int j = 0; j < n; ++j)
        {
            x_new[i] += H[i][j] * x[j];
        }
    }
}

double maxDiff(const vector<double>& a, const vector<double>& b)
{
    double max_val = 0.0;

    for (int i = 0; i < a.size(); ++i)
    {
        max_val = max(max_val, abs(a[i] - b[i]));
    }
    return max_val;
}

vector<double> solveMPI(const vector<vector<double>>& H, const vector<double>& g, double hNorm)
{
    int n = H.size();
    vector<double> x = g;
    vector<double> x_prev(n);
    double f = hNorm / (1 - hNorm);
    int iter = 0;

    do {
        x_prev = x;
        iterate(H, g, x_prev, x);
        iter++;
    } while (f * maxDiff(x, x_prev) >= C);

    cout << "Iterations (Parallel): " << iter << endl;
    return x;
}

int main()
{
    auto start = chrono::high_resolution_clock::now();


    vector<vector<double>> A =
    {
     {10, 1, 2, 0, 1},
     {1, 15, 3, 2, 0},
     {2, 3, 20, 1, 1},
     {0, 2, 1, 18, 2},
     {1, 0, 1, 2, 12}
    };

    // Вектор b = A * [1, 1, 1, 1, 1]
    vector<double> b = { 14, 21, 27, 23, 16 };

    vector<vector<double>> H;
    vector<double> g;
    convertToHForm(A, b, H, g);

    double hNorm = computeHNorm(H);
    if (hNorm >= 1)
    {
        cerr << "Method diverges: ||H|| = " << hNorm << endl;
        return 1;
    }

    vector<double> solution = solveMPI(H, g, hNorm);

    auto end = chrono::high_resolution_clock::now();  // Конец замера
    chrono::duration<double> time = end - start;

    cout << "Solution: ";
    for (double val : solution) cout << val << " ";
    cout << "\nTime (Parallel): " << time.count() << " sec" << endl;

    return 0;
}