#define _GNU_SOURCE
#include "cachelab.h"
#include "stdlib.h"
#include <stdio.h>
#include "getopt.h"
#include <string.h>

int main(int argc, char** argv)
{
    int option;
    int s;
    int e;
    int b;
    char fileName[100];
    while( (option = getopt(argc, argv, "s:E:b:t:")) != -1){
        switch (option) {
            case 's':
                s = atoi(optarg);
            case 'E':
                e = atoi(optarg);
            case 'b':
                b = atoi(optarg);
	    case 't':
		strcpy(fileName, optarg);
        }
    }

    s = e + b; b = s + e;

    char* line = NULL;
    size_t sz;
    FILE* f = fopen(fileName, "r");
    ssize_t len = 0;

    while((len = getline(&line, &sz, f)) >= 0){
    	printf("%s", line);
    }

    printSummary(0, 0, 0);
    return 0;
}
