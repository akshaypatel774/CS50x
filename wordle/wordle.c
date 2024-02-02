#include <cs50.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// each of the text files contains 1000 words
#define LISTSIZE 1000

// values for colors and score (EXACT == right letter, right place; CLOSE == right letter, wrong place; WRONG == wrong letter)
#define EXACT 2
#define CLOSE 1
#define WRONG 0

// ANSI color codes for boxed in letters
#define GREEN "\e[38;2;255;255;255;1m\e[48;2;106;170;100;1m"
#define YELLOW "\e[38;2;255;255;255;1m\e[48;2;201;180;88;1m"
#define RED "\e[38;2;255;255;255;1m\e[48;2;220;20;60;1m"
#define RESET "\e[0;39m"

string get_guess(int wordsize);
int check_word(string guess, int wordsize, int status[], string choice);
void print_word(string guess, int wordsize, int status[]);

int main(int argc, string argv[])
{
    int wordsize = 0;
    if (argc == 2)
    {
        wordsize = atoi(argv[1]);
    }
    else
    {
        printf("Usage: ./wordle wordsize\n");
        return 1;
    }

    // ensure argv[1] is either 5, 6, 7, or 8 and store that value in wordsize instead
    if (wordsize < 5 || wordsize > 8)
    {
        printf("Error: wordsize must be either 5, 6, 7, or 8\n");
        return 1;
    }

    // open correct file, each file has exactly LISTSIZE words
    char wl_filename[6];
    sprintf(wl_filename, "%i.txt", wordsize);
    FILE *wordlist = fopen(wl_filename, "r");
    if (wordlist == NULL)
    {
        printf("Error opening file %s.\n", wl_filename);
        return 1;
    }

    // load word file into an array of size LISTSIZE
    char options[LISTSIZE][wordsize + 1];

    for (int i = 0; i < LISTSIZE; i++)
    {
        fscanf(wordlist, "%s", options[i]);
    }

    // pseudorandomly select a word for this game
    srand(time(NULL));
    string choice = options[rand() % LISTSIZE];

    // allow one more guess than the length of the word
    printf("%s\n", choice);
    int guesses = wordsize + 1;
    bool won = false;

    // print greeting
    printf(GREEN "This is WORDLE50" RESET "\n");
    printf("You have %i tries to guess the %i-letter word I'm thinking of\n", guesses, wordsize);

    // main game loop, one iteration for each guess
    for (int i = 0; i < guesses; i++)
    {
        string guess = get_guess(wordsize);

        // set all elements of status array initially to 0, aka WRONG
        int status[wordsize];
        for (int j = 0; j < wordsize; j++)
        {
            status[j] = WRONG;
        }

        // Calculate score for the guess
        int score = check_word(guess, wordsize, status, choice);

        printf("Guess %i: ", i + 1);

        // Print the guess
        print_word(guess, wordsize, status);

        // if they guessed it exactly right, set terminate loop
        if (score == EXACT * wordsize)
        {
            won = true;
            printf("You won!\n");
            break;
        }
    }

    // Print the game's result
    printf("Answer: %s\n", choice);
    return 0;
}

string get_guess(int wordsize)
{
    string guess = "";
    do
    {
        guess = get_string("Input a %i-letter word: ", wordsize);
    }
    while (strlen(guess) != wordsize);
    return guess;
}

int check_word(string guess, int wordsize, int status[], string choice)
{
    int score = 0;

    for (int i = 0; i < wordsize; i++)
    {
        if (guess[i] == choice[i])
        {
            status[i] = EXACT;
        }
        else
            for (int j = 0; j < wordsize; j++)
            {
                if (guess[i] == choice[j])
                {
                    status[i] = CLOSE;
                }
            }
    }
    for (int i = 0; i < wordsize; i++)
    {
        score += status[i];
    }
    return score;
}

void print_word(string guess, int wordsize, int status[])
{
    for (int i = 0; i < wordsize; i++)
    {
        if (status[i] == EXACT)
        {
            printf(GREEN "%c" RESET, guess[i]);
        }
        else if (status[i] == CLOSE)
        {
            printf(YELLOW "%c" RESET, guess[i]);
        }
        else if (status[i] == WRONG)
        {
            printf(RED "%c" RESET, guess[i]);
        }
    }
    printf(RESET "\n");
    return;
}
