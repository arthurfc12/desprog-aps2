#include <math.h>

#include "fourier.h"

void nft(double complex s[MAX_SIZE], double complex t[MAX_SIZE], int n, int sign)
{
    for (int k = 0; k < n; k++)
    {
        t[k] = 0;

        for (int j = 0; j < n; j++)
        {
            t[k] += s[j] * cexp(sign * 2 * PI * k * j * I / n);
        }
    }
}

void nft_forward(double complex s[MAX_SIZE], double complex t[MAX_SIZE], int n)
{
    nft(s, t, n, -1);
}

void nft_inverse(double complex t[MAX_SIZE], double complex s[MAX_SIZE], int n)
{
    nft(t, s, n, 1);

    for (int k = 0; k < n; k++)
    {
        s[k] /= n;
    }
}

void fft(double complex s[MAX_SIZE], double complex t[MAX_SIZE], int n, int sign)
{
    double complex si[MAX_SIZE];
    double complex sp[MAX_SIZE];
    double complex tp[MAX_SIZE];
    double complex ti[MAX_SIZE];
    int contp = 0;
    int conti = 0;

    // CONDIÇÃO PARA A RECURSIVA NÃO ENTRAR EM LOOP INFINITO
    if (n < 2)
    {
        t[0] = s[0];
        return;
    }

    //for para separar sinal s em 2: si e sp (par e impar)
    for (int k = 0; k < n; k++)
    {
        if (fmod(k, 2) == 0)
        {
            sp[contp] = s[k];
            contp++;
        }
        else
        {
            si[conti] = s[k];
            conti++;
        }
    }

    // calcular o tp e ti atraves da recursiva
    fft(sp, tp, n / 2, sign);
    fft(si, ti, n / 2, sign);

    // calcular o t final com o tp e ti calculados anteriormente

    // primeira parte da formula
    for (int j = 0; j < n; j++)
    {
        t[j] = tp[j] + ti[j] * cexp(sign * 2 * PI * j * I / n);
    }

    // segunda parte da formula
    for (int i = 0; i < n; i++)
    {
        t[i + (n / 2)] = tp[i] - ti[i] * cexp(sign * 2 * PI * i * I / n);
    }
}

void fft_forward(double complex s[MAX_SIZE], double complex t[MAX_SIZE], int n)
{
    fft(s, t, n, -1);
}

void fft_inverse(double complex t[MAX_SIZE], double complex s[MAX_SIZE], int n)
{
    fft(t, s, n, 1);

    for (int k = 0; k < n; k++)
    {
        s[k] /= n;
    }
}

void fft_forward_2d(double complex matrix[MAX_SIZE][MAX_SIZE], int width, int height)
{
    double complex linha[MAX_SIZE];
    double complex linha_saida[MAX_SIZE];
    double complex coluna[MAX_SIZE];
    double complex coluna_saida[MAX_SIZE];

    //linha fixa
    for (int j = 0; j < height; j++)
    {
        //coluna
        for (int i = 0; i < width; i++)
        {
            linha[i] = matrix[j][i];
        }

        fft_forward(linha, linha_saida, width);
        //coluna
        for (int c = 0; c < width; c++)
        {
            matrix[j][c] = linha_saida[c];
        }
    }
    //coluna fixa
    for (int x = 0; x < width; x++)
    {
        //linha
        for (int i = 0; i < height; i++)
        {
            coluna_saida[i] = matrix[i][x];
        }

        fft_forward(coluna_saida, coluna, height);
        //linha
        for (int l = 0; l < height; l++)
        {
            matrix[l][x] = coluna[l];
        }
    }
}

void fft_inverse_2d(double complex matrix[MAX_SIZE][MAX_SIZE], int width, int height)
{
    double complex linha[MAX_SIZE];
    double complex linha_saida[MAX_SIZE];
    double complex coluna[MAX_SIZE];
    double complex coluna_saida[MAX_SIZE];

    //linha fixa
    for (int j = 0; j < height; j++)
    {
        //coluna
        for (int i = 0; i < width; i++)
        {
            linha[i] = matrix[j][i];
        }

        fft_inverse(linha, linha_saida, width);
        //coluna
        for (int c = 0; c < width; c++)
        {
            matrix[j][c] = linha_saida[c];
        }
    }
    //coluna fixa
    for (int x = 0; x < width; x++)
    {
        //linha
        for (int i = 0; i < height; i++)
        {
            coluna_saida[i] = matrix[i][x];
        }

        fft_inverse(coluna_saida, coluna, height);
        //linha
        for (int l = 0; l < height; l++)
        {
            matrix[l][x] = coluna[l];
        }
    }
}

void filter(double complex input[MAX_SIZE][MAX_SIZE], double complex output[MAX_SIZE][MAX_SIZE], int width, int height, int flip)
{
    int center_x = width / 2;
    int center_y = height / 2;

    double variance = -2 * SIGMA * SIGMA;

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            int dx = center_x - (x + center_x) % width;
            int dy = center_y - (y + center_y) % height;

            double d = dx * dx + dy * dy;

            double g = exp(d / variance);

            if (flip)
            {
                g = 1 - g;
            }

            output[y][x] = g * input[y][x];
        }
    }
}

void filter_lp(double complex input[MAX_SIZE][MAX_SIZE], double complex output[MAX_SIZE][MAX_SIZE], int width, int height)
{
    filter(input, output, width, height, 0);
}

void filter_hp(double complex input[MAX_SIZE][MAX_SIZE], double complex output[MAX_SIZE][MAX_SIZE], int width, int height)
{
    filter(input, output, width, height, 1);
}
