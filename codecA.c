#include "codecA.h"

void codecA(char *input)
{
    if (input == NULL)
    {
        return;
    }
    char *pointer = input;
    while (*pointer != '\0')
    {
        if (*pointer >= 'a' && *pointer <= 'z')
        {
            *pointer = *pointer - 32; // to upper case
        }
        else if (*pointer >= 'A' && *pointer <= 'Z')
        {
            *pointer = *pointer + 32; // to lower case
        }
        pointer++;
    }
};

void codecA_encode(char *data)
{
    codecA(data);
};

void codecA_decode(char *data)
{
    codecA(data);
};

