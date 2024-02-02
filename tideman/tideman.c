#include <cs50.h>
#include <stdio.h>
#include <string.h>

// Max number of candidates
#define MAX 9

// preferences[i][j] is number of voters who prefer i over j
int preferences[MAX][MAX];

// locked[i][j] means i is locked in over j
bool locked[MAX][MAX];

// Each pair has a winner, loser
typedef struct
{
    int winner;
    int loser;
} pair;

// Array of candidates
string candidates[MAX];
pair pairs[MAX * (MAX - 1) / 2];

int pair_count;
int candidate_count;

// Function prototypes
bool vote(int rank, string name, int ranks[]);
void record_preferences(int ranks[]);
void add_pairs(void);
void sort_pairs(void);
void lock_pairs(void);
void print_winner(void);

void create_strength_array(int strength[]);
void sort(int array[], int start, int end);
void merge(int array[], int start, int middle, int end);
void print_array(int array[], int array_size);
bool check_cycle(int winner, int loser);

int main(int argc, string argv[])
{
    // Check for invalid usage
    if (argc < 2)
    {
        printf("Usage: tideman [candidate ...]\n");
        return 1;
    }

    // Populate array of candidates
    candidate_count = argc - 1;
    if (candidate_count > MAX)
    {
        printf("Maximum number of candidates is %i\n", MAX);
        return 2;
    }
    for (int i = 0; i < candidate_count; i++)
    {
        candidates[i] = argv[i + 1];
    }

    // Clear graph of locked in pairs
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            locked[i][j] = false;
        }
    }

    pair_count = 0;
    int voter_count = get_int("Number of voters: ");

    // Query for votes
    for (int i = 0; i < voter_count; i++)
    {
        // ranks[i] is voter's ith preference
        int ranks[candidate_count];

        // Query for each rank
        for (int j = 0; j < candidate_count; j++)
        {
            string name = get_string("Rank %i: ", j + 1);

            if (!vote(j, name, ranks))
            {
                printf("Invalid vote.\n");
                return 3;
            }
        }

        record_preferences(ranks);

        printf("\n");
    }

    add_pairs();
    sort_pairs();
    lock_pairs();
    print_winner();
    return 0;
}

// Update ranks given a new vote
bool vote(int rank, string name, int ranks[])
{
    for (int i = 0; i < candidate_count; i++)
    {
        if (strcmp(candidates[i], name) == 0)
        {
            ranks[rank] = i;
            return true;
        }
    }
    return false;
}

// Update preferences given one voter's ranks
void record_preferences(int ranks[])
{
    int first = 0;
    int second = 0;
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = i + 1; j < candidate_count; j++)
        {
            first = ranks[i];
            second = ranks[j];
            if (first != second)
            {

                preferences[first][second]++;
            }
            else
            {
                preferences[first][second] = 0;
            }
        }
    }
    return;
}

// Record pairs of candidates where one is preferred over the other
void add_pairs(void)
{
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = i + 1; j < candidate_count; j++)
        {
            int first = preferences[i][j];
            int second = preferences[j][i];
            if (first > second)
            {
                pairs[pair_count].winner = i;
                pairs[pair_count].loser = j;
                pair_count++;
            }
            else if (first < second)
            {
                pairs[pair_count].winner = j;
                pairs[pair_count].loser = i;
                pair_count++;
            }
        }
    }
    return;
}

// Sort pairs in decreasing order by strength of victory
void sort_pairs(void)
{
    int strength_array[pair_count];
    create_strength_array(strength_array);
    sort(strength_array, 0, pair_count - 1);
    return;
}

// Lock pairs into the candidate graph in order, without creating cycles
void lock_pairs(void)
{
    int winner;
    int loser;
    for (int i = 0; i < pair_count; i++)
    {
        winner = pairs[i].winner;
        loser = pairs[i].loser;
        if (!check_cycle(winner, loser))
        {
            locked[winner][loser] = true;
        }
    }
    return;
}

// Print the winner of the election
void print_winner(void)
{
    int i = 0;
    int j = 0;
    bool edge;
    while (i < candidate_count)
    {
        edge = locked[j][i];
        if (j == candidate_count && !edge)
        {
            printf("%s\n", candidates[i]);
            break;
        }
        else if (!edge)
        {
            j++;
        }
        else
        {
            i++;
            j = 0;
        }
    }
    return;
}

void create_strength_array(int strength_array[])
{
    int winner;
    int loser;
    for (int i = 0; i < pair_count; i++)
    {
        winner = pairs[i].winner;
        loser = pairs[i].loser;
        strength_array[i] = preferences[winner][loser] - preferences[loser][winner];
    }
}

void sort(int array[], int start, int end)
{
    if (end > start)
    {
        int middle = (start + end) / 2;

        sort(array, start, middle);
        sort(array, middle + 1, end);

        merge(array, start, middle, end);
    }
}

void merge(int array[], int start, int middle, int end)
{
    int i = start;
    int j = middle + 1;
    int k = 0;

    typedef struct
    {
        int winner;
        int loser;
    } temp_pair;

    temp_pair temp_pairs[pair_count];
    while (i <= middle && j <= end)
    {
        if (array[i] >= array[j])
        {
            temp_pairs[k].winner = pairs[i].winner;
            temp_pairs[k].loser = pairs[i].loser;
            i++;
            k++;
        }
        else
        {
            temp_pairs[k].winner = pairs[j].winner;
            temp_pairs[k].loser = pairs[j].loser;
            j++;
            k++;
        }
    }

    while (i <= middle)
    {
        temp_pairs[k].winner = pairs[i].winner;
        temp_pairs[k].loser = pairs[i].loser;
        i++;
        k++;
    }

    while (j <= end)
    {
        temp_pairs[k].winner = pairs[j].winner;
        temp_pairs[k].loser = pairs[j].loser;
        j++;
        k++;
    }
    for (i = start; i <= end; i++)
    {
        pairs[i].winner = temp_pairs[i - start].winner;
        pairs[i].loser = temp_pairs[i - start].loser;
    }
}

void print_array(int array[], int array_size)
{
    for (int i = 0; i < array_size; i++)
    {
        printf("%i ", array[i]);
    }
    printf("\n");
}

bool check_cycle(int winner, int loser)
{
    if (locked[loser][winner] == true)
    {
        return true;
    }

    for (int i = 0; i < candidate_count; i++)
    {
        if (locked[i][winner] == true)
        {
            return check_cycle(i, loser);
        }
    }
    return false;
}
