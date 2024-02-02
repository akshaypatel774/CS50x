#include "helpers.h"
#include <math.h>
#include <stdio.h>

int maxSize(int bit)
{
    if (bit > 255)
    {
        return 255;
    }
    else
    {
        return bit;
    }
}

// Convert image to grayscale
void grayscale(int height, int width, RGBTRIPLE image[height][width])
{
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            float avg = round((image[i][j].rgbtRed + image[i][j].rgbtGreen + image[i][j].rgbtBlue) / 3.0);
            image[i][j].rgbtRed = avg;
            image[i][j].rgbtGreen = avg;
            image[i][j].rgbtBlue = avg;
        }
    }
}

// Reflect image horizontally
void reflect(int height, int width, RGBTRIPLE image[height][width])
{
    for (int i = 0; i < height; i++)
    {
        RGBTRIPLE tmp[width];
        for (int j = 0; j < width; j++)
        {
            tmp[j] = image[i][j];
        }
        for (int j = 0; j < width; j++)
        {
            image[i][j].rgbtRed = tmp[width - 1 - j].rgbtRed;
            image[i][j].rgbtGreen = tmp[width - 1 - j].rgbtGreen;
            image[i][j].rgbtBlue = tmp[width - 1 - j].rgbtBlue;
        }
    }
}

// Blur image
void blur(int height, int width, RGBTRIPLE image[height][width])
{
    RGBTRIPLE tmp_image[height][width];
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            tmp_image[i][j] = image[i][j];
        }
    }
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            float totalRed = 0;
            float totalGreen = 0;
            float totalBlue = 0;
            int count = 0;

            for (int k = -1; k < 2; k++)
            {

                for (int l = -1; l < 2; l++)
                {
                    if (i + k < 0 || i + k >= height)
                    {
                        continue;
                    }
                    if (j + l < 0 || j + l >= width)
                    {
                        continue;
                    }
                    totalRed += tmp_image[i + k][j + l].rgbtRed;
                    totalGreen += tmp_image[i + k][j + l].rgbtGreen;
                    totalBlue += tmp_image[i + k][j + l].rgbtBlue;
                    count++;
                }
            }
            image[i][j].rgbtRed = (int) round(totalRed / count);
            image[i][j].rgbtGreen = (int) round(totalGreen / count);
            image[i][j].rgbtBlue = (int) round(totalBlue / count);
        }
    }
}

// Detect edges
void edges(int height, int width, RGBTRIPLE image[height][width])
{
    RGBTRIPLE tmp_image[height][width];
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            tmp_image[i][j] = image[i][j];
        }
    }

    int Gx[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
    int Gy[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            float GxRed = 0;
            float GxGreen = 0;
            float GxBlue = 0;
            float GyRed = 0;
            float GyGreen = 0;
            float GyBlue = 0;

            for (int k = -1; k < 2; k++)
            {

                for (int l = -1; l < 2; l++)
                {
                    if (i + k < 0 || i + k >= height)
                    {
                        continue;
                    }
                    if (j + l < 0 || j + l >= width)
                    {
                        continue;
                    }
                    GxRed += tmp_image[i + k][j + l].rgbtRed * Gx[k + 1][l + 1];
                    GxGreen += tmp_image[i + k][j + l].rgbtGreen * Gx[k + 1][l + 1];
                    GxBlue += tmp_image[i + k][j + l].rgbtBlue * Gx[k + 1][l + 1];
                    GyRed += tmp_image[i + k][j + l].rgbtRed * Gy[k + 1][l + 1];
                    GyGreen += tmp_image[i + k][j + l].rgbtGreen * Gy[k + 1][l + 1];
                    GyBlue += tmp_image[i + k][j + l].rgbtBlue * Gy[k + 1][l + 1];
                }
            }
            image[i][j].rgbtRed = maxSize(round(sqrt(GxRed * GxRed + GyRed * GyRed)));
            image[i][j].rgbtGreen = maxSize(round(sqrt(GxGreen * GxGreen + GyGreen * GyGreen)));
            image[i][j].rgbtBlue = maxSize(round(sqrt(GxBlue * GxBlue + GyBlue * GyBlue)));
        }
    }
}
