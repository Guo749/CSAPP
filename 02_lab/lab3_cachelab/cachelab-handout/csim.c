#include "cachelab.h"
#include "unistd.h"
#include "stdlib.h"
#include "stdio.h"

int main(int argc, char** argv)
{
    int option;
    int s;
    int e;
    int b;
    while( (option = getopt(argc, argv, "seb")) != -1){
        switch (option) {
            case 's':
                s = atoi(optarg);
            case 'e':
                e = atoi(optarg);
            case 'b':
                b = atoi(optarg);
        }
    }

    printf("%d %d %d\n", s, e, b);

    printSummary(0, 0, 0);
    return 0;
}
