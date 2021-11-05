#include "cachelab.h"
#include "stdlib.h"
#include "stdio.h"
#include "getopt.h"

int main(int argc, char** argv)
{
    int option;
    int s;
    int e;
    int b;
    while( (option = getopt(argc, argv, "s:E:b:")) != -1){
        switch (option) {
            case 's':
                s = atoi(optarg);
            case 'E':
                e = atoi(optarg);
            case 'b':
                b = atoi(optarg);
        }
    }

    printf("%d %d %d\n", s, e, b);

    printSummary(0, 0, 0);
    return 0;
}
