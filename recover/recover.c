#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define BUFFER 512

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Usage: ./recover image\n");
        return 1;
    }
    FILE *card = fopen(argv[1], "r");
    if (card == NULL)
    {
        printf("Could not open file.\n");
        return 1;
    }
    uint8_t *buffer = malloc(BUFFER);
    if (buffer == NULL)
    {
        fclose(card);
        printf("Memory allocation failed.\n");
        return 1;
    }
    char filename[8];
    int photoCount = 0;

    FILE *imageFile = NULL;

    while (fread(buffer, sizeof(unsigned char), BUFFER, card) == BUFFER)
    {
        if (buffer[0] == 0xff && buffer[1] == 0xd8 && buffer[2] == 0xff && (buffer[3] & 0xf0) == 0xe0)
        {
            if (imageFile != NULL)
            {
                fclose(imageFile);
            }

            sprintf(filename, "%03i.jpg", photoCount);
            imageFile = fopen(filename, "w");
            if (imageFile == NULL)
            {
                free(buffer);
                fclose(card);
                printf("Could not create image file.\n");
                return 1;
            }
            fwrite(buffer, 1, BUFFER, imageFile);
            photoCount++;
        }
        else if (imageFile != NULL)
        {
            fwrite(buffer, 1, BUFFER, imageFile);
        }
    }

    if (imageFile != NULL)
    {
        fclose(imageFile);
    }

    free(buffer);
    fclose(card);
    printf("Total images recovered: %i\n", photoCount);
    return 0;
}
