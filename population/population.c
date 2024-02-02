#include <cs50.h>
#include <stdio.h>

int calculate_years(int start_population, int end_population);

int main(void)
{
    int start_population;
    int end_population;

    // TODO: Prompt for start size
    do
    {
        start_population = get_int("Start size: ");
    }
    while (start_population < 9);

    // TODO: Prompt for end size
    do
    {
        end_population = get_int("End size: ");
    }
    while (end_population < start_population);

    // TODO: Calculate number of years until we reach threshold
    int years = calculate_years(start_population, end_population);

    // TODO: Print number of years
    printf("Years: %i\n", years);
}


int calculate_years(int start_population, int end_population)
{
    int birth;
    int death;
    int year;
    for (year = 0; start_population < end_population; year++)
    {
        birth = start_population / 3;
        death = start_population / 4;
        start_population = start_population + birth - death;
    }
    return year;
}