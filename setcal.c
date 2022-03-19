#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "string.h"

// constants for magic values
#define FILEMODE "r"
#define UNIVERSE 'U'
#define SET 'S'
#define RELATION 'R'
#define CALCULATE 'C'
#define SPACE " "
#define MAX_LENGTH 30
#define MAX_LINES 1000
#define PAIR 2

// constants for set commands
#define EMPTY "empty"
#define CARD "card"
#define COMPLEMENT "complement"
#define UNION "union"
#define INTERSECT "intersect"
#define MINUS "minus"
#define SUBSETEQ "subseteq"
#define SUBSET "subset"
#define EQUALS "equals"
// constants for relation commands
#define REFLEXIVE "reflexive"
#define SYMMETRIC "symmetric"
#define ANTISYMMETRIC "antisymmetric"
#define TRANSITIVE "transitive"
#define FUNCTION "function"
#define DOMAIN "domain"
#define CODOMAIN "codomain"
#define INJECTIVE "injective"
#define SURJECTIVE "surjective"
#define BIJECTIVE "bijective"

#define _TRUE "true"
#define _FALSE "false"

// macro for allocating and reallocating
#define allocate(var, size) ((var = malloc(size)) == NULL)
#define reallocate(errorPointer, var, size) ((errorPointer = realloc(var, size)) == NULL)

#define RESTRICTED_COUNT 21
// restricted constants
const char *RESTRICTED[RESTRICTED_COUNT] = {
    EMPTY,
    CARD,
    COMPLEMENT,
    UNION,
    INTERSECT,
    MINUS,
    SUBSETEQ,
    SUBSET,
    EQUALS,
    _TRUE,
    _FALSE,
    REFLEXIVE,
    SYMMETRIC,
    ANTISYMMETRIC,
    TRANSITIVE,
    FUNCTION,
    DOMAIN,
    CODOMAIN,
    INJECTIVE,
    SURJECTIVE,
    BIJECTIVE,
};

enum ERROR // error codes
{
    MEMORY_ERR = 1,
    ARGS_ERR,
    FILE_ERR
};
// error handler
void ExitFailure(int error)
{
    printf("ERROR: ");
    switch (error)
    {
    case MEMORY_ERR:
        fprintf(stderr, "Unable to allocate more memory");
        break;
    case ARGS_ERR:
        fprintf(stderr, "Invalid Arguments");
        break;
    case FILE_ERR:
        fprintf(stderr, "Cannot open file");
        break;
    }
    exit(error);
}

// structures

typedef struct // set
{
    char **items;
    int numberOfItems;
} set_t;

typedef struct // pair
{
    char elements[PAIR][MAX_LENGTH];
} pair_t;

typedef struct // relation
{
    pair_t *pairs;
    int numberOfPairs;
} rel_t;

typedef struct // expression
{
    char *operation;
    int argumentsCount;
    int *arguments;
} cal_t;

typedef struct // line
{
    void *ref;
    char type;

} line_t;

// prototypes
set_t *Save(char *str, int *errorCode);
rel_t *SaveRelation(char *str, int *errorCode);
void ProcessCal(line_t *lines, char *str, int *errorCode);

// additional functions
void __complement(set_t *set1, set_t *set2);
void __minus(set_t *set1, set_t *set2);
bool __func(rel_t *rel, set_t *universe);
bool __inject(rel_t *rel, set_t *set1, set_t *set2);
bool __inSet(rel_t *rel, set_t *set, int numberOfElement);

// set functions
void SetEmpty(set_t *set);
void SetCard(set_t *set);
void SetComplement(set_t *set, set_t *universe);
void SetUnion(set_t *set1, set_t *set2);
void SetIntersect(set_t *set1, set_t *set2);
void SetMinus(set_t *set1, set_t *set2);
void SetSubseteq(set_t *set1, set_t *set2);
void SetSubset(set_t *set1, set_t *set2);
void SetEquals(set_t *set1, set_t *set2);

// relation functions
void RelReflexive(rel_t *rel, set_t *universe);
void RelSymmetric(rel_t *rel);
void RelAntisymmetric(rel_t *rel);
void RelTransitive(rel_t *rel);
void RelFunction(rel_t *rel, set_t *universe);
void RelCodomain(rel_t *rel, set_t *universe);
void RelDomain(rel_t *rel, set_t *universe);
void RelInjective(rel_t *rel, set_t *set1, set_t *set2);
void RelBijective(rel_t *rel, set_t *set1, set_t *set2);
void RelSurjective(rel_t *rel, set_t *set1, set_t *set2);

// validators
int ValidatesStringSet(char *str, void **ref, set_t *universe);
int ValidatesStringRelation(char *str, void **ref, set_t *universe);
// start
int main(int argc, char *argv[])
{
    // if program was launched with invalid params
    if (argc != 2)
        ExitFailure(ARGS_ERR);

    FILE *fp; // file pointer

    // opens file
    if ((fp = fopen(argv[1], FILEMODE)) == NULL)
        ExitFailure(FILE_ERR);

    int numberOfLines = 0; // number of lines for array lines
    int ch;                // char to read file by symbol
    int sizeOfLine = 0;    // length of each line

    line_t *lines; // lines from the file (pointers to sets and relations)
    char *line;    // variable to store each line while reading the file

    if (allocate(lines, sizeof(line_t))) // allocates memory for lines
        return MEMORY_ERR;
    if (allocate(line, sizeof(char))) // allocates memory for line
        return MEMORY_ERR;

    void *errorCheck;  // error pointer
    int errorCode = 0; // error code

    int setBegin = 0;     // index where set or relation starts
    int commandBegin = 0; // index where command starts

    while ((ch = fgetc(fp)) != EOF)
    {
        if (ch == '\n')
        {
            line[sizeOfLine] = '\0';
            sizeOfLine = 0;

            int length = strlen(line);

            if ((length != 1 && line[1] != ' ') || line[length - 1] == ' ') // if string has space at the beginning and at the end
            {
                errorCode = ARGS_ERR;
                break;
            }
            if ((!numberOfLines && line[0] != UNIVERSE)) // if the first set is not universum
            {
                errorCode = ARGS_ERR;
                break;
            }

            if (line[0] == UNIVERSE ||
                line[0] == SET)
            {
                setBegin = numberOfLines; // assigns index of set
                lines[numberOfLines].type = SET;

                if ((numberOfLines && line[0] == UNIVERSE)) // if we found the second universum
                {
                    errorCode = ARGS_ERR;
                    break;
                }

                // validates and creates new object of set and put its pointer into the lines
                if ((errorCode = ValidatesStringSet(line, &lines[numberOfLines].ref, (line[0] == UNIVERSE ? NULL : (set_t *)lines[0].ref))))
                    break;

                /*  prints out set  */
                if (line[0] == SET)
                    printf("\n");

                putchar(line[0] == UNIVERSE ? UNIVERSE : SET);

                for (int i = 0; i < ((set_t *)(lines[numberOfLines].ref))->numberOfItems; i++)
                    printf(" %s", ((set_t *)(lines[numberOfLines].ref))->items[i]);

                numberOfLines++;
            }
            else if (line[0] == RELATION)
            {
                setBegin = numberOfLines; // assigns index of relation
                lines[numberOfLines].type = line[0];

                // validates and creates new object of relation and put its pointer into the lines
                if ((errorCode = ValidatesStringRelation(line, &lines[numberOfLines].ref, (set_t *)lines[0].ref)))
                    break;
                /*  prints out relation  */
                putchar('\n');
                putchar(RELATION);
                for (int i = 0; i < ((rel_t *)(lines[numberOfLines].ref))->numberOfPairs; i++)
                {
                    printf(" (%s", ((rel_t *)(lines[numberOfLines].ref))->pairs[i].elements[0]);
                    printf(" %s)", ((rel_t *)(lines[numberOfLines].ref))->pairs[i].elements[1]);
                }

                numberOfLines++;
            }
            else if (line[0] == CALCULATE)
            {
                commandBegin = numberOfLines; // assigns index of command

                putchar('\n');
                ProcessCal(lines, line, &errorCode); // executes commands
            }
            else
            {
                errorCode = ARGS_ERR;
            }

            // if user declared sets or relations after commands
            if (commandBegin && commandBegin == setBegin)
            {
                errorCode = ARGS_ERR;
                break;
            }
            if (errorCode)
                break;
        }
        else
        {
            line[sizeOfLine++] = ch;

            // resizes line
            if (reallocate(errorCheck, line, (sizeOfLine + 1) * sizeof(line_t)))
            {
                break;
            }
            line = errorCheck; // if there wasnt any error, then error check has reference to newly reallocated memory
        }

        if (numberOfLines >= MAX_LINES)
        {
            errorCode = ARGS_ERR;
            break;
        }
        // resizes lines
        if (reallocate(errorCheck, lines, (numberOfLines + 1) * sizeof(line_t)))
        {
            errorCode = MEMORY_ERR;
            break;
        }
        lines = errorCheck;
    }

    // if user declared 1 line or didn't declare any commands
    if (numberOfLines < 2 || !commandBegin)
    {
        errorCode = ARGS_ERR;
    }

    // memory free
    free(line);
    for (int i = 0; i < numberOfLines; i++)
    {
        if (lines[i].type == SET)
        {
            for (int j = 0; j < ((set_t *)lines[i].ref)->numberOfItems; j++)
            {
                free(((set_t *)lines[i].ref)->items[j]);
            }
            free(((set_t *)lines[i].ref)->items);
        }
        if (lines[i].type == RELATION)
        {

            free(((rel_t *)lines[i].ref)->pairs);
        }
    }
    for (int i = 0; i < numberOfLines; i++)
        free(lines[i].ref); // frees each pointer to array (string) in lines
    free(lines);            // frees pointer of poiters
    fclose(fp);             // close file

    if (errorCode)
        ExitFailure(errorCode);
    return errorCode;
}

// converts string into set
set_t *Save(char *str, int *errorCode)
{
    set_t *set; // variable to store new set
    char *pch;  // variable to keep results of strtok

    if (allocate(set, sizeof(set_t))) // allocates memory for set
    {
        *errorCode = MEMORY_ERR;
        return NULL;
    }
    set->numberOfItems = 0;                   // resets count of the elements
    if (allocate(set->items, sizeof(char *))) // allocates memory for set items
    {
        free(set);
        *errorCode = MEMORY_ERR;
        return NULL;
    }
    void *error; // error pointer

    pch = strtok(str, SPACE);

    while ((pch = strtok(NULL, SPACE)) != NULL)
    {
        if (strlen(pch) > MAX_LENGTH) // if length of pch is greater than 30
        {
            *errorCode = ARGS_ERR;
            break;
        }
        for (int i = 0; i < RESTRICTED_COUNT; i++) // if element is one of restricted words
        {
            if (!strcmp(pch, RESTRICTED[i]))
            {
                *errorCode = ARGS_ERR;
                break;
            }
        }
        if (allocate(set->items[set->numberOfItems], strlen(pch) + 1)) // allocates memory for one element
        {
            *errorCode = MEMORY_ERR;
            break;
        }
        strcpy(set->items[set->numberOfItems], pch); // copies element into the object
        set->numberOfItems++;

        // resizes items of set
        if (reallocate(error, set->items, (set->numberOfItems + 1) * sizeof(char *)))
        {
            *errorCode = MEMORY_ERR;
            break;
        }
        set->items = error;
    }

    // free

    // if there is an error
    if (*errorCode)
    {
        for (int i = 0; i < set->numberOfItems; i++)
            free(set->items[i]);
        free(set->items);
        free(set);

        return NULL;
    }

    return set;
}

// prints set
void PrintSet(set_t *set) //??
{
    putchar(SET);
    for (int i = 0; i < set->numberOfItems; i++)
    {
        printf(" %s", set->items[i]);
    }
}

// checks if elements in set are equal
int CheckSameElems(set_t *set)
{
    for (int i = 0; i < set->numberOfItems - 1; i++)
        for (int j = i + 1; j < set->numberOfItems; j++)
            if (!strcmp(set->items[i], set->items[j]))
            {
                return ARGS_ERR;
            }
    return 0;
}

// validates set
int ValidatesStringSet(char *str, void **ref, set_t *universe)
{

    int length = strlen(str);

    int errorCode = 0;
    if (universe != NULL && str[0] == UNIVERSE) // if universum isn't null and the string is for universum it means that it's the second universum
    {

        return ARGS_ERR;
    }
    if (length == 1)
    {
        *ref = Save(str, &errorCode); // creates pointer to the structure

        return EXIT_SUCCESS;
    }
    for (int i = 1; i < length; i++) // if string has digits and etc..
    {
        if ((str[i] == ' ' && str[i - 1] == ' ') || !((str[i] >= 'A' && str[i] <= 'Z') || (str[i] >= 'a' && str[i] <= 'z') || str[i] == ' ')) // if string has 2 spaces in a row
        {
            errorCode = ARGS_ERR;
            break;
        }
    }

    if (errorCode) // if there was an error
    {
        return errorCode;
    }

    *ref = Save(str, &errorCode); // creates pointer to the structure

    set_t *set = (set_t *)*ref;
    if (errorCode) // if there was an error
    {
        return errorCode;
    }

    // checks if set has same elements
    if ((errorCode = CheckSameElems(set)))
    {
        for (int i = 0; i < set->numberOfItems; i++)
        {
            free(set->items[i]);
        }
        free(set->items);
        free(set);
        return errorCode;
    }

    // checks if all the elements are in universum
    if (universe != NULL)
    {
        for (int i = 0; i < set->numberOfItems; i++)
        {
            bool fl = false;
            for (int j = 0; j < universe->numberOfItems; j++)
                if ((fl = !strcmp(set->items[i], universe->items[j])))
                    break;
            if (!fl)
            {
                for (int i = 0; i < set->numberOfItems; i++)
                {
                    free(set->items[i]);
                }
                free(set->items);
                free(set);
                printf("14");
                return ARGS_ERR;
            }
        }
    }
    return EXIT_SUCCESS;
}

// validates string relation
int ValidatesStringRelation(char *str, void **ref, set_t *universe)
{

    int length = strlen(str);
    int errorCode = 0; // error code
    bool fl;           // flag

    if (length == 1)
    {
        *ref = SaveRelation(str, &errorCode); // creates a pointer to an object

        return EXIT_SUCCESS;
    }
    for (int i = 1; i < length; i++)
    {
        if ((str[i] == ' ' && str[i - 1] == ' ') || !((str[i] >= 'A' && str[i] <= 'Z') || (str[i] >= 'a' && str[i] <= 'z') || str[i] == ' ' || str[i] == '(' || str[i] == ')')) // if string has 2 spaces in a row
        {
            errorCode = ARGS_ERR;
            break;
        }
    }

    // if there is an error
    if (errorCode)
        return errorCode;

    *ref = SaveRelation(str, &errorCode); // creates a pointer to an object

    rel_t *rel = (rel_t *)*ref;

    if (errorCode)
        return errorCode;
    // checks if relation has the same pairs
    for (int i = 0; i < rel->numberOfPairs; i++)
    {
        for (int j = i + 1; j < rel->numberOfPairs; j++)
        {
            if (!strcmp(rel->pairs[i].elements[0], rel->pairs[j].elements[0]) &&
                !strcmp(rel->pairs[i].elements[1], rel->pairs[j].elements[1]))
            {
                free(rel->pairs);
                free(rel);
                return ARGS_ERR;
            }
        }
    }

    // checks if all the elements of pairs in relation are in universe
    for (int i = 0; i < rel->numberOfPairs; i++)
    {
        for (int k = 0; k < PAIR; k++)
        {
            fl = false;
            for (int j = 0; j < universe->numberOfItems; j++)
            {
                if ((fl = !strcmp(rel->pairs[i].elements[k], universe->items[j])))
                {
                    break;
                }
            }
            if (!fl)
            {

                free(rel->pairs);
                free(rel);
                return ARGS_ERR;
            }
        }
    }
    return EXIT_SUCCESS;
}

// converts string into relation
rel_t *SaveRelation(char *str, int *errorCode)
{
    char *pch;
    rel_t *rel;

    if (allocate(rel, sizeof(rel_t)))
    {
        *errorCode = MEMORY_ERR;
        return NULL;
    }
    if (allocate(rel->pairs, sizeof(pair_t)))
    {
        *errorCode = MEMORY_ERR;
        return NULL;
    }

    void *error; // error pointer

    pch = strtok(str, SPACE);

    int numOfElems = 0;     // number of pairs in relation
    rel->numberOfPairs = 0; // resets number of pairs

    while ((pch = strtok(NULL, SPACE)) != NULL)
    {
        if (strlen(pch) - 1 > MAX_LENGTH)
        {
            *errorCode = ARGS_ERR;
            break;
        }

        if (numOfElems % 2 == 0)
        {
            // validates pair in relation
            if (pch[0] != '(')
            {
                *errorCode = ARGS_ERR;
                break;
            }
            strcpy(rel->pairs[rel->numberOfPairs].elements[0], pch);

            // removes ( from the first element
            for (int i = 0; i < (int)strlen(pch); i++)
                rel->pairs[rel->numberOfPairs].elements[0][i] = rel->pairs[rel->numberOfPairs].elements[0][i + 1];
            for (int i = 0; i < RESTRICTED_COUNT; i++)
            {
                if (!strcmp(RESTRICTED[i], rel->pairs[rel->numberOfPairs].elements[0]))
                {
                    *errorCode = ARGS_ERR;
                    break;
                }
            }
        }
        else
        {
            // removes ) from the second element
            if (pch[strlen(pch) - 1] != ')')
            {
                *errorCode = ARGS_ERR;
                break;
            }
            strcpy(rel->pairs[rel->numberOfPairs].elements[1], pch);
            rel->pairs[rel->numberOfPairs++].elements[1][strlen(pch) - 1] = '\0'; // delete the right bracket

            for (int i = 0; i < RESTRICTED_COUNT; i++)
            {
                if (!strcmp(RESTRICTED[i], rel->pairs[rel->numberOfPairs - 1].elements[1]))
                {
                    *errorCode = ARGS_ERR;
                    break;
                }
            }
            // resizes pairs array
            if (reallocate(error, rel->pairs, (rel->numberOfPairs + 1) * sizeof(pair_t)))
            {
                *errorCode = MEMORY_ERR;
                break;
            }
            rel->pairs = error;
        }
        numOfElems++;
    }

    // if there was an error
    if (*errorCode)
    {
        free(rel->pairs);
        free(rel);
        return NULL;
    }

    return rel;
}

// calculates sets & rels
void ProcessCal(line_t *lines, char *str, int *errorCode)
{
    int num;     // temp argument
    char *pch;   // pointer for strtok
    void *error; // error pointer

    cal_t cal; // object for expressions

    cal.argumentsCount = 0; // resets argument count

    pch = strtok(str, SPACE);

    if ((pch = strtok(NULL, SPACE)) != NULL)
    {
        if (allocate(cal.operation, strlen(pch) + 1)) // allocates memory for operation string
        {
            *errorCode = MEMORY_ERR;
            return;
        }
        strcpy(cal.operation, pch);
    }

    if (allocate(cal.arguments, sizeof(int))) // allocates memory for arguments
    {
        *errorCode = MEMORY_ERR;
        return;
    }

    // reads arguments of the command
    while ((pch = strtok(NULL, SPACE)) != NULL)
    {
        if ((num = atoi(pch)) != 0)
        {
            cal.arguments[cal.argumentsCount++] = num;
            if (reallocate(error, cal.arguments, (cal.argumentsCount + 1) * sizeof(int *)))
            {
                *errorCode = MEMORY_ERR;
                break;
            }
            cal.arguments = error;
        }
    }

    // command execution
    if (!strcmp(cal.operation, EMPTY) && (cal.argumentsCount == 1) && ((lines[cal.arguments[0] - 1].type == SET)))
        SetEmpty((set_t *)(lines[cal.arguments[0] - 1].ref));
    else if (!strcmp(cal.operation, COMPLEMENT) && (cal.argumentsCount == 1) && ((lines[cal.arguments[0] - 1].type == SET)))
        SetComplement((set_t *)(lines[cal.arguments[0] - 1].ref), (set_t *)(lines[0].ref));
    else if (!strcmp(cal.operation, CARD) && (cal.argumentsCount == 1) && ((lines[cal.arguments[0] - 1].type == SET)))
        SetCard((set_t *)(lines[cal.arguments[0] - 1].ref));
    else if (!strcmp(cal.operation, UNION) && (cal.argumentsCount == 2) && ((lines[cal.arguments[0] - 1].type == SET)) && (lines[cal.arguments[1] - 1].type == SET))
        SetUnion((set_t *)(lines[cal.arguments[0] - 1].ref), (set_t *)(lines[cal.arguments[1] - 1].ref));
    else if (!strcmp(cal.operation, INTERSECT) && (cal.argumentsCount == 2) && ((lines[cal.arguments[0] - 1].type == SET)) && (lines[cal.arguments[1] - 1].type == SET))
        SetIntersect((set_t *)(lines[cal.arguments[0] - 1].ref), (set_t *)(lines[cal.arguments[1] - 1].ref));
    else if (!strcmp(cal.operation, MINUS) && (cal.argumentsCount == 2) && ((lines[cal.arguments[0] - 1].type == SET)) && (lines[cal.arguments[1] - 1].type == SET))
        SetMinus((set_t *)(lines[cal.arguments[0] - 1].ref), (set_t *)(lines[cal.arguments[1] - 1].ref));
    else if (!strcmp(cal.operation, SUBSETEQ) && (cal.argumentsCount == 2) && ((lines[cal.arguments[0] - 1].type == SET)) && (lines[cal.arguments[1] - 1].type == SET))
        SetSubseteq((set_t *)(lines[cal.arguments[0] - 1].ref), (set_t *)(lines[cal.arguments[1] - 1].ref));
    else if (!strcmp(cal.operation, SUBSET) && (cal.argumentsCount == 2) && ((lines[cal.arguments[0] - 1].type == SET)) && (lines[cal.arguments[1] - 1].type == SET))
        SetSubset((set_t *)(lines[cal.arguments[0] - 1].ref), (set_t *)(lines[cal.arguments[1] - 1].ref));
    else if (!strcmp(cal.operation, EQUALS) && (cal.argumentsCount == 2) && ((lines[cal.arguments[0] - 1].type == SET)) && (lines[cal.arguments[1] - 1].type == SET))
        SetEquals((set_t *)(lines[cal.arguments[0] - 1].ref), (set_t *)(lines[cal.arguments[1] - 1].ref));
    else if (!strcmp(cal.operation, REFLEXIVE) && (cal.argumentsCount == 1) && (lines[cal.arguments[0] - 1].type == RELATION))
        RelReflexive((rel_t *)(lines[cal.arguments[0] - 1].ref), (set_t *)(lines[0].ref));
    else if (!strcmp(cal.operation, SYMMETRIC) && (cal.argumentsCount == 1) && (lines[cal.arguments[0] - 1].type == RELATION))
        RelSymmetric((rel_t *)(lines[cal.arguments[0] - 1].ref));
    else if (!strcmp(cal.operation, ANTISYMMETRIC) && (cal.argumentsCount == 1) && (lines[cal.arguments[0] - 1].type == RELATION))
        RelAntisymmetric((rel_t *)(lines[cal.arguments[0] - 1].ref));
    else if (!strcmp(cal.operation, TRANSITIVE) && (cal.argumentsCount == 1) && (lines[cal.arguments[0] - 1].type == RELATION))
        RelTransitive((rel_t *)(lines[cal.arguments[0] - 1].ref));
    else if (!strcmp(cal.operation, FUNCTION) && (cal.argumentsCount == 1) && (lines[cal.arguments[0] - 1].type == RELATION))
        RelFunction((rel_t *)(lines[cal.arguments[0] - 1].ref), (set_t *)(lines[0].ref));
    else if (!strcmp(cal.operation, DOMAIN) && (cal.argumentsCount == 1) && (lines[cal.arguments[0] - 1].type == RELATION))
        RelDomain((rel_t *)(lines[cal.arguments[0] - 1].ref), (set_t *)(lines[0].ref));
    else if (!strcmp(cal.operation, CODOMAIN) && (cal.argumentsCount == 1) && (lines[cal.arguments[0] - 1].type == RELATION))
        RelCodomain((rel_t *)(lines[cal.arguments[0] - 1].ref), (set_t *)(lines[0].ref));
    else if (!strcmp(cal.operation, INJECTIVE) && (cal.argumentsCount == 3) && (lines[cal.arguments[0] - 1].type == RELATION))
        RelInjective((rel_t *)(lines[cal.arguments[0] - 1].ref), (set_t *)(lines[cal.arguments[1] - 1].ref), (set_t *)(lines[cal.arguments[2] - 1].ref));
    else if (!strcmp(cal.operation, SURJECTIVE) && (cal.argumentsCount == 3) && (lines[cal.arguments[0] - 1].type == RELATION))
        RelSurjective((rel_t *)(lines[cal.arguments[0] - 1].ref), (set_t *)(lines[cal.arguments[1] - 1].ref), (set_t *)(lines[cal.arguments[2] - 1].ref));
    else if (!strcmp(cal.operation, BIJECTIVE) && (cal.argumentsCount == 3) && (lines[cal.arguments[0] - 1].type == RELATION))
        RelBijective((rel_t *)(lines[cal.arguments[0] - 1].ref), (set_t *)(lines[cal.arguments[1] - 1].ref), (set_t *)(lines[cal.arguments[2] - 1].ref));
    else
    {
        free(cal.operation);
        free(cal.arguments);
        *errorCode = ARGS_ERR;
        return;
    }

    free(cal.operation);
    free(cal.arguments);
}

// returns true if set is empty
void SetEmpty(set_t *set)
{
    printf((set->numberOfItems) ? _FALSE : _TRUE);
}

// returns number of elements from set
void SetCard(set_t *set)
{
    printf("%d", set->numberOfItems);
}

// component for Minus
void __minus(set_t *set1, set_t *set2)
{
    bool fl;
    for (int i = 0; i < set1->numberOfItems; i++)
    {
        fl = true;
        for (int j = 0; j < set2->numberOfItems; j++)
        {
            if (strcmp(set2->items[j], set1->items[i]) == 0)
            {
                fl = false;
                break;
            }
        }
        if (fl)
            printf(" %s", set1->items[i]);
    }
}

// component for Complement
void __complement(set_t *set1, set_t *set2)
{
    if (set1->numberOfItems != 0)
    {
        __minus(set2, set1);
    }
    else
    {
        for (int i = 0; i < set2->numberOfItems; i++)
        {
            printf(" %s", set2->items[i]);
        }
    }
}

// Complement
void SetComplement(set_t *set, set_t *universe)
{
    putchar(SET);
    __complement(set, universe);
}

// Union
void SetUnion(set_t *set1, set_t *set2)
{
    putchar(SET);
    for (int i = 0; i < set1->numberOfItems; i++)
        printf(" %s", set1->items[i]);
    __complement(set1, set2);
}

// Intersect
void SetIntersect(set_t *set1, set_t *set2)
{
    putchar(SET);
    for (int i = 0; i < set1->numberOfItems; i++)
        for (int j = 0; j < set2->numberOfItems; j++)
            if (!strcmp(set1->items[i], set2->items[j]))
                printf(" %s", set1->items[i]);
}

// Minus
void SetMinus(set_t *set1, set_t *set2)
{
    putchar(SET);
    __minus(set1, set2);
}

// Subseteq
void SetSubseteq(set_t *set1, set_t *set2)
{
    bool fl;
    if (set1->numberOfItems == 0)
    {
        printf(_TRUE);
        return;
    }
    if (set2->numberOfItems == 0)
    {
        printf(_FALSE);
        return;
    }
    for (int i = 0; i < set1->numberOfItems; i++)
    {
        fl = true;
        for (int j = 0; j < set2->numberOfItems; j++)
        {
            if (strcmp(set2->items[j], set1->items[i]) == 0)
            {
                fl = false;
                break;
            }
        }
        if (fl)
        {
            printf(_FALSE);
            return;
        }
    }
    printf(_TRUE);
}
// Subset
void SetSubset(set_t *set1, set_t *set2)
{
    bool fl;
    if (set2->numberOfItems == 0)
    {
        printf(_FALSE);
        return;
    }
    if (set1->numberOfItems == 0)
    {
        printf(_TRUE);
        return;
    }

    for (int i = 0; i < set1->numberOfItems; i++)
    {
        fl = true;
        for (int j = 0; j < set2->numberOfItems; j++)
        {
            if (strcmp(set2->items[j], set1->items[i]) == 0)
            {
                fl = false;
                break;
            }
        }
        if (fl)
        {
            printf(_FALSE);
            return;
        }
    }
    if (set2->numberOfItems == set1->numberOfItems)
        printf(_FALSE);
    else
        printf(_TRUE);
}

// Equals
void SetEquals(set_t *set1, set_t *set2)
{
    bool fl;

    if (set2->numberOfItems == 0 && set1->numberOfItems == 0)
    {
        printf(_TRUE);
        return;
    }
    if (set2->numberOfItems != set1->numberOfItems)
    {
        printf(_FALSE);
        return;
    }
    for (int i = 0; i < set1->numberOfItems; i++)
    {
        fl = false;
        for (int j = 0; j < set2->numberOfItems; j++)
            if ((fl = !strcmp(set2->items[j], set1->items[i])))
                break;
        if (!fl)
        {
            printf(_FALSE);
            return;
        }
    }
    printf(_TRUE);
}

// Reflexive
void RelReflexive(rel_t *rel, set_t *universe)
{
    int count = 0;
    for (int i = 0; i < rel->numberOfPairs; i++)
        if (strcmp(rel->pairs[i].elements[0], rel->pairs[i].elements[1]) == 0)
            count++;
    if (count == universe->numberOfItems)
        printf(_TRUE);
    else
        printf(_FALSE);
}

// Symmetric
void RelSymmetric(rel_t *rel)
{
    bool fl;
    for (int i = 0; i < (rel->numberOfPairs); i++)
    {
        if (strcmp(rel->pairs[i].elements[0], rel->pairs[i].elements[1]) == 0)
        {
            continue;
        }
        fl = true;

        for (int j = 0; j < rel->numberOfPairs; j++)
        {
            if ((strcmp(rel->pairs[i].elements[0], rel->pairs[j].elements[1]) == 0) && (strcmp(rel->pairs[i].elements[1], rel->pairs[j].elements[0]) == 0))
            {
                fl = false;
                break;
            }
        }
        if (fl)
        {
            printf(_FALSE);
            return;
        }
    }
    printf(_TRUE);
}

/*
 * Returns true or false if relation is Antisymmetric
 *
 */
void RelAntisymmetric(rel_t *rel)
{
    for (int i = 0; i < (rel->numberOfPairs); i++)
    {
        if (strcmp(rel->pairs[i].elements[0], rel->pairs[i].elements[1]) == 0)
        {
            continue;
        }
        for (int j = 0; j < rel->numberOfPairs; j++)
        {
            if ((strcmp(rel->pairs[i].elements[0], rel->pairs[j].elements[1]) == 0) && (strcmp(rel->pairs[i].elements[1], rel->pairs[j].elements[0]) == 0))
            {
                printf(_FALSE);
                return;
            }
        }
    }
    printf(_TRUE);
}

/*
 * Returns true or false if relation is Transitive
 *
 */
void RelTransitive(rel_t *rel)
{
    bool fl;
    for (int i = 0; i < (rel->numberOfPairs); i++)
    {
        if (strcmp(rel->pairs[i].elements[0], rel->pairs[i].elements[1]) == 0)
        {
            continue;
        }
        for (int j = 0; j < rel->numberOfPairs; j++)
            if (strcmp(rel->pairs[i].elements[1], rel->pairs[j].elements[0]) == 0)
            {
                fl = true;
                for (int k = 0; k < rel->numberOfPairs; k++)
                    if (((strcmp(rel->pairs[i].elements[0], rel->pairs[k].elements[0]) == 0) && (strcmp(rel->pairs[j].elements[1], rel->pairs[k].elements[1]) == 0)))
                    {
                        fl = false;
                        break;
                    }
                if (fl)
                {
                    printf(_FALSE);
                    return;
                }
            }
    }
    printf(_TRUE);
}

bool __func(rel_t *rel, set_t *universe)
{
    int count;
    for (int i = 0; i < (universe->numberOfItems); i++)
    {
        count = 0;
        for (int j = 0; j < rel->numberOfPairs; j++)
        {
            if (strcmp(universe->items[i], rel->pairs[j].elements[0]) == 0)
                count++;
        }
        if (count > 1)
        {
            return false;
        }
    }
    return true;
}

// Function
void RelFunction(rel_t *rel, set_t *universe)
{
    printf(__func(rel, universe) ? _TRUE : _FALSE);
}

// Domain
void RelDomain(rel_t *rel, set_t *universe)
{
    putchar(SET);
    for (int i = 0; i < universe->numberOfItems; i++)
    {
        for (int j = 0; j < rel->numberOfPairs; j++)
        {
            if (strcmp(universe->items[i], rel->pairs[j].elements[0]) == 0)
            {
                printf(" %s", universe->items[i]);
                break;
            }
        }
    }
}

// Codomain
void RelCodomain(rel_t *rel, set_t *universe)
{
    putchar(SET);
    for (int i = 0; i < universe->numberOfItems; i++)
    {
        for (int j = 0; j < rel->numberOfPairs; j++)
        {
            if (strcmp(universe->items[i], rel->pairs[j].elements[1]) == 0)
            {
                printf(" %s", universe->items[i]);
                break;
            }
        }
    }
}

// component of injective function
bool __inject(rel_t *rel, set_t *set1, set_t *set2)
{
    if (__func(rel, set1))
    {
        int count;
        for (int i = 0; i < (set2->numberOfItems); i++)
        {
            count = 0;
            for (int j = 0; j < rel->numberOfPairs; j++)
            {
                if (strcmp(set2->items[i], rel->pairs[j].elements[1]) == 0)
                    count++;
            }
            if (count > 1)
            {
                return false;
            }
        }
        return true;
    }
    return false;
}

bool __inSet(rel_t *rel, set_t *set, int numberOfElement)
{
    bool fl;
    for (int i = 0; i < rel->numberOfPairs; i++)
    {
        for (int j = 0; j < set->numberOfItems; j++)
        {
            if ((fl = !strcmp(rel->pairs[i].elements[numberOfElement], set->items[j])))
                break;
        }
        if (!fl)
            return false;
    }
    return true;
}

void RelInjective(rel_t *rel, set_t *set1, set_t *set2)
{
    if ((__inSet(rel, set1, 0) && (__inSet(rel, set2, 1))))
        printf(__inject(rel, set1, set2) ? _TRUE : _FALSE);
    else
        printf(_FALSE);
}

void RelSurjective(rel_t *rel, set_t *set1, set_t *set2)
{
    if ((__inSet(rel, set1, 0) && (__inSet(rel, set2, 1) && (__func(rel, set1)))))
        printf(_TRUE);
    else
        printf(_FALSE);
}

void RelBijective(rel_t *rel, set_t *set1, set_t *set2)
{
    if ((__inSet(rel, set1, 0) && (__inSet(rel, set2, 1))))
        printf(__inject(rel, set1, set2) ? _TRUE : _FALSE);
    else
        printf(_FALSE);
}