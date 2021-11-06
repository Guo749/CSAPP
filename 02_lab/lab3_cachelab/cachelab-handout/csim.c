#define _GNU_SOURCE
#include "cachelab.h"
#include "stdlib.h"
#include <stdio.h>
#include "getopt.h"
#include <string.h>
#define addrLen 8

static int S;
static int E;
static int B;
static int hits = 0;
static int misses = 0;
static int evictions = 0;


typedef struct _Node{
    unsigned tag;
    struct _Node* next;
    struct _Node* prev;
}Node;

typedef struct _LRU{
    Node* head;
    Node* tail;
    int size;
}LRU;

static LRU* lru;

void initializeLRU(int i){
    lru[i].head = malloc(sizeof(Node));
    lru[i].tail = malloc(sizeof(Node));

    lru[i].head->next = lru[i].tail;
    lru[i].tail->prev = lru[i].head;
    lru[i].size       = 0;
}

/**
 *
 * @param lru the lru we manupilate
 * @param pos the node position to be deleted usually 0 or 1
 *              0 means the first one
 *              1 means the last one
 */
void deleteElement(unsigned set, Node* nodeToDel){
    nodeToDel->next->prev = nodeToDel->prev;
    nodeToDel->prev->next = nodeToDel->next;
    lru->size--;
}

void evict(unsigned set){
    deleteElement(set, lru->tail->prev);
}

void addFirst(unsigned set, Node* node){
    node->next = lru->head->next;
    node->prev = lru->head;

    lru->head->next->prev = node;
    lru->head->next       = node;

    lru->size += 1;
}

void parseOption(int argc, char** argv, char** fileName){
    int option;
    while( (option = getopt(argc, argv, "s:E:b:t:")) != -1){
        switch (option) {
            case 's':
                S = atoi(optarg);
            case 'E':
                E = atoi(optarg);
            case 'b':
                B = atoi(optarg);
            case 't':
                strcpy(*fileName, optarg);
        }
    }
}

void update(unsigned address){
    unsigned mask = 0xFFFFFFFF;
    unsigned maskSet = mask >> (32 - S);
    unsigned targetSet = (maskSet) & (address >> B);
    unsigned targetTag = mask & (address >> (S + B));

    LRU curLru = lru[targetSet];
    if(curLru.size == 2){ // full, need to evict
        deleteElement(targetSet, curLru.tail->prev);
        Node* newNode = malloc(sizeof(Node));
        newNode->tag = targetTag;
        addFirst(targetSet, newNode);

        evictions++;
        misses++;
    }else{
        Node* cur = curLru.head->next;
        int found = 0;
        while(cur != curLru.tail){
            if(cur->tag == targetTag){
                found = 1;
            }

            cur = cur->next;
        }

        if(found){
            hits++;
            deleteElement(targetSet, cur);
            addFirst(targetSet, cur);
        }else{
            misses++;
            addFirst(targetSet, cur);
        }
    }
}


void cacheSimulateWhole(char* fileName){
    // step1: new lru with s sets
    lru = malloc(S * sizeof(*lru));
    for(int i = 0; i < S; i++)
        initializeLRU(i);

    FILE* file = fopen(fileName, "r");
    char op;
    unsigned address;
    int size;
    // L 10, 1
    while(fscanf(file, " %c %x,%d", &op, &address, &size) > 0){
        switch (op) {
            case 'L':
                update(address);
                break;
            case 'M':
                update(address);
            case 'S':
                update(address);
                break;
        }
    }
}

int main(int argc, char** argv)
{
    char* fileName = malloc(100 * sizeof(char));

    // step1: parse option
    parseOption(argc, argv, &fileName);

    // step2: read all of the lines and analyze it
    cacheSimulateWhole(fileName);

    printSummary(hits, misses, evictions);
    return 0;
}
