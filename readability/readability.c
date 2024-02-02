#include <cs50.h>
#include <math.h>
#include <stdio.h>

int main(void)
{

    string text = get_string("Text: ");

    // Calculate letters, words and sentences in the text
    int n = 0;
    int letters = 0;
    int words = 1;
    int sentences = 0;

    while (text[n] != '\0')
    {
        if (text[n] == '.' || text[n] == '!' || text[n] == '?')
        {
            sentences += 1;
        }
        if (text[n] == ' ')
        {
            words += 1;
        }
        if (text[n] >= 'A' && text[n] <= 'z')
        {
            letters += 1;
        }
        n++;
    }

    // Calculate index
    float L = (100.0 * letters) / words;
    float S = (100.0 * sentences) / words;

    int index = round(0.0588 * L - 0.296 * S - 15.8);

    // Print results
    if (index > 15)
    {
        printf("Grade 16+\n");
    }
    else if (index < 1)
    {
        printf("Before Grade 1\n");
    }
    else
    {
        printf("Grade %i\n", index);
    }
}
