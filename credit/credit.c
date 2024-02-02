#include <cs50.h>
#include <stdio.h>

int main(void)
{
    long card;
    int sum = 0;
    int count = 0;

    do
    {
        card = get_long("Number: ");
    }
    while (card <= 0);

    // Get first 2 digits
    long firstTwoDigits = card;
    while (firstTwoDigits >= 100)
    {
        firstTwoDigits /= 10;
    }

    int firstDigit = firstTwoDigits / 10;

    // Validation Process
    while (card > 0)
    {
        int digit = card % 10;
        if (count % 2 == 1)
        {
            digit *= 2;
            while (digit > 0)
            {
                sum += digit % 10;
                digit /= 10;
            }
        }
        else
        {
            sum += digit;
        }
        card /= 10;
        count++;
    }

    // Print results
    if (sum % 10 == 0)
    {
        if ((firstTwoDigits == 34 || firstTwoDigits == 37) && count == 15)
        {
            printf("AMEX\n");
        }
        else if (firstDigit == 4 && (count == 13 || count == 16))
        {
            printf("VISA\n");
        }
        else if (firstTwoDigits > 50 && firstTwoDigits < 56)
        {
            printf("MASTERCARD\n");
        }
        else
        {
            printf("INVALID\n");
        }
    }
    else
    {
        printf("INVALID\n");
    }
}
