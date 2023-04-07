#include "codecB.h"

void codecB_encode(char *data)
{
    if (data == NULL)
    {
        return;
    }
    char *pointer = data;
    while (*pointer != '\0')
    {
        *pointer = *pointer + 3;
        pointer++;
    }
};

void codecB_decode(char *data)
{
    if (data == NULL)
    {
        return;
    }
    char *pointer = data;
    while (*pointer != '\0')
    {
        *pointer = *pointer - 3;
        pointer++;
    }
};