#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define N 2          // Problem dimension (example)
#define M 5          // Memory size
#define MAX_ITER 100 // Max iterations
#define EPS 1e-6     // Convergence tolerance
#define ALPHA 0.1    // Step size (fixed step)

// Objective function (Rosenbrock function example)
double objective(const double *x)
{
    return 100.0 * pow(x[1] - x[0] * x[0], 2) + pow(1 - x[0], 2);
}

// Gradient of the objective function
void gradient(const double *x, double *g)
{
    g[0] = -400.0 * x[0] * (x[1] - x[0] * x[0]) + 2.0 * (x[0] - 1);
    g[1] = 200.0 * (x[1] - x[0] * x[0]);
}

// Dot product
double dot(const double *a, const double *b, int size)
{
    double result = 0.0;
    for (int i = 0; i < size; i++)
    {
        result += a[i] * b[i];
    }
    return result;
}

// Scale vector
void scale(double *v, double alpha, int size)
{
    for (int i = 0; i < size; i++)
    {
        v[i] *= alpha;
    }
}

// Copy vector
void copy(double *dest, const double *src, int size)
{
    for (int i = 0; i < size; i++)
    {
        dest[i] = src[i];
    }
}

// Add two vectors: res = a + alpha * b
void add(double *res, const double *a, const double *b, double alpha, int size)
{
    for (int i = 0; i < size; i++)
    {
        res[i] = a[i] + alpha * b[i];
    }
}

// L-BFGS Optimization
void lbfgs(double *x)
{
    double s[M][N], y[M][N]; // Past step & gradient changes
    double rho[M];           // rho[i] = 1 / (y[i]^T * s[i])
    double alpha[M], beta[M];
    double g[N], g_prev[N], p[N], x_prev[N], q[N];

    gradient(x, g);

    for (int iter = 0; iter < MAX_ITER; iter++)
    {
        copy(q, g, N);

        int bound = (iter < M) ? iter : M;

        // Compute rho[i] before using it
        for (int i = 0; i < bound; i++)
        {
            double ys = dot(y[i], s[i], N);
            rho[i] = (fabs(ys) > 1e-10) ? 1.0 / ys : 0.0; // Avoid division by zero
        }

        // First loop (compute direction)
        for (int i = bound - 1; i >= 0; i--)
        {
            alpha[i] = rho[i] * dot(s[i], q, N);
            add(q, q, y[i], -alpha[i], N);
        }

        // Approximate Hessian scaling (H0 = gamma * I)
        double gamma = 1.0;
        if (iter > 0)
        {
            int last = (iter - 1) % M;
            gamma = dot(s[last], y[last], N) / dot(y[last], y[last], N);
        }
        scale(q, gamma, N);

        // Second loop (apply corrections)
        for (int i = 0; i < bound; i++)
        {
            beta[i] = rho[i] * dot(y[i], q, N);
            add(q, q, s[i], alpha[i] - beta[i], N);
        }

        // Search direction p = -q
        scale(q, -1, N);
        copy(p, q, N);

        // Line search (fixed step size for simplicity)
        copy(x_prev, x, N);
        copy(g_prev, g, N);
        add(x, x, p, ALPHA, N);

        gradient(x, g);

        // Check for convergence (norm of gradient)
        if (sqrt(dot(g, g, N)) < EPS)
        {
            printf("Converged in %d iterations.\n", iter + 1);
            return;
        }

        // Store s = x_k+1 - x_k and y = g_k+1 - g_k
        int idx = iter % M;
        copy(s[idx], x, N);
        add(s[idx], s[idx], x_prev, -1, N);
        copy(y[idx], g, N);
        add(y[idx], y[idx], g_prev, -1, N);
    }
}

int main()
{
    double x[N] = {-1.2, 1.0}; // Initial guess
    lbfgs(x);
    printf("Optimal solution: x = (%f, %f)\n", x[0], x[1]);
    return 0;
}
