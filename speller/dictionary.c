// Implements a dictionary's functionality
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "dictionary.h"

// Represents a node in a hash table
typedef struct node
{
    char word[LENGTH + 1];
    struct node *next;
} node;

// TODO: Choose number of buckets in hash table
const unsigned int N = 3000;

// Hash table
node *table[N];

// Word Count
int word_count = 0;

// Returns true if word is in dictionary, else false
bool check(const char *word)
{
    unsigned int hash_value = hash(word);

    node *ptr = table[hash_value];
    while (ptr != NULL)
    {
        if (strcasecmp(ptr->word, word) == 0)
        {
            return true;
        }
        else
        {
            ptr = ptr->next;
        }
    }
    return false;
}

// Hashes word to a number
unsigned int hash(const char *word)
{
    // Hash function
    unsigned int hash = 5381;
    int c;

    while ((c = tolower(*word++)))
    {
        hash = ((hash << 5) + hash) ^ c;
    }

    return hash % N;
}

// Loads dictionary into memory, returning true if successful, else false
bool load(const char *dictionary)
{
    FILE *source = fopen(dictionary, "r");

    if (source == NULL)
    {
        return false;
    }

    char word[LENGTH + 1];
    while (fscanf(source, "%s", word) != EOF)
    {
        node *new_node = malloc(sizeof(node));
        if (new_node == NULL)
        {
            unload();
            return false;
        }

        strcpy(new_node->word, word);
        new_node->next = NULL;

        unsigned int hash_value = hash(new_node->word);

        node *head = table[hash_value];

        // Insert the new node at the beginning
        if (head == NULL)
        {
            table[hash_value] = new_node;
            word_count++;
        }
        else
        {
            new_node->next = table[hash_value];
            table[hash_value] = new_node;
            word_count++;
        }
    }
    fclose(source);
    return true;
}

// Returns number of words in dictionary if loaded, else 0 if not yet loaded
unsigned int size(void)
{
    return word_count;
}

// Unloads dictionary from memory, returning true if successful, else false
bool unload(void)
{
    for (int i = 0; i < N; i++)
    {
        node *head = table[i];
        node *ptr = head;
        node *tmp = head;

        while (ptr != NULL)
        {
            ptr = ptr->next;
            free(tmp);
            tmp = ptr;
        }
    }
    return true;
}
