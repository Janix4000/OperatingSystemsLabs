#include <stdio.h>
#include "library.h"

#include <stdlib.h>

int main()
{
    printf("Hello World! Magic number is %d!\n", test_function());
    FILE *file = fopen("test.txt", "r");
    if (file)
    {
        printf("Number of lines: %d\n", getNumberOfLines(file));
        fclose(file);
    }
    return 0;
}