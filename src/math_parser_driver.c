#include "math_parser.h"

int main(int argc, char * argv[])
{
    char buffer[3000];

    while (fgets(buffer, 3000, stdin))
    {
        printf("%d\n", math_eval(buffer));
    }

    return 0;
}