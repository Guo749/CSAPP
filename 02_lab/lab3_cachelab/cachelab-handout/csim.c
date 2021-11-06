#define _GNU_SOURCE
#include "cachelab.h"
#include "stdlib.h"
#include <stdio.h>
#include "getopt.h"
#include <string.h>
#define addrLen 8

char hex[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E'};
char* bin[16] = {"0000", "0001", "0010", "0011", "0100","0101","0110", "0111", "1000", "1001", "1010", "1011", "1100", "1101", "1110", "1111"};

typedef struct _Node{
    char tag[100];
    struct _Node* next;
    struct _Node* prev;
}Node;

void initializeNode(Node* node, char* tag){
    strcpy(node->tag, tag);
}

typedef struct _LRU{
    Node* head;
    Node* tail;
    int size;
}LRU;

Node* new(size_t size){
    Node* res = malloc(size);
    res->prev = NULL;
    res->next = NULL;
    res->tag[0] = '@';
    return res;
}


void initializeLRU(LRU* lru){
    lru->head = new(sizeof(lru->head));
    lru->tail = new(sizeof(lru->tail));

    lru->head->next = lru->tail;
    lru->tail->prev = lru->head;
    lru->size       = 0;
}

/**
 *
 * @param lru the lru we manupilate
 * @param pos the node position to be deleted usually 0 or 1
 *              0 means the first one
 *              1 means the last one
 */
void deleteElement(LRU* lru, Node* nodeToDel){
    nodeToDel->next->prev = nodeToDel->prev;
    nodeToDel->prev->next = nodeToDel->next;
    lru->size--;
}

void evict(LRU* lru){
    deleteElement(lru, lru->tail->prev);
}

void addFirst(LRU* lru, Node* node){
    node->next = lru->head->next;
    node->prev = lru->head;

    lru->head->next->prev = node;
    lru->head->next       = node;

    lru->size += 1;
}

void parseOption(int* s, int* e, int* b, int argc, char** argv, char** fileName){
    int option;
    while( (option = getopt(argc, argv, "s:E:b:t:")) != -1){
        switch (option) {
            case 's':
                *s = atoi(optarg);
            case 'E':
                *e = atoi(optarg);
            case 'b':
                *b = atoi(optarg);
            case 't':
                strcpy(*fileName, optarg);
        }
    }
}

void cacheSimulateOne(LRU* curLRU, char* tag, char commandType, int* hits, int* misses, int* evicitons){
    Node* curNode = curLRU->head->next;
    int find = 0;
    while(curNode != curLRU->tail){
        if(strcmp(curNode->tag, tag) == 0){
            find = 1;
            break;
        }

        curNode = curNode->next;
    }

    if(find == 1){
        if(commandType == 'M'){
            *hits += 2;
        }else{
            *hits += 1;
        }

        //update
        deleteElement(curLRU, curNode);
        addFirst(curLRU, curNode);
    }else{
        Node* node = malloc(sizeof(Node));
        initializeNode(node, tag);


        if(curLRU->size == 2){
            evict(curLRU);
            addFirst(curLRU, node);
            *evicitons += 1;
            *hits += 1;
            *misses += 1;
        }else{
            addFirst(curLRU, node);
            *misses += 1;
            *hits += 1;
        }

        if(commandType == 'M'){
            *hits += 1;
        }

    }
}


void cacheSimulateWhole(char* fileName, int* hits, int* misses, int* evicitons, int s, int b){
    // step1: new lru with s sets
    LRU lru[s];
    for(int i = 0; i < s; i++)
        initializeLRU(&lru[i]);

    // step2: generate

    char* line = NULL;
    size_t sz;
    FILE* f = fopen(fileName, "r");
    ssize_t len = 0;

    while((len = getline(&line, &sz, f)) >= 0){
        char* addr = malloc(addrLen * sizeof(*addr));
        char* commaPos = strchr(line, ',');
        int space;
        char commandType;

        if(line[0] == ' '){ //start with M or L or S
            space = 1;
            commandType = line[1];
        }else{  //start with I
            space = 2;
            commandType = 'I';
        }

        int index = 0;
        for(int i = space; i < space + addrLen; i++){
            addr[index++] = line[i];
        }
        addr[index] = '\0';

        char fullBinary[64];
        int fullIndex = 0;
        for(int j = 0; j < addrLen; j++){
            for(int k = 0; k < 4; k++){
                fullBinary[fullIndex++] = bin[j][k];
            }
        }

        int startPosSet  = 64 - b - s;
        char tag[100];
        int setVal = 0;
        for(int i = 0; i < s; i++){
            setVal += fullBinary[startPosSet++] - '0';
            setVal <<= 1;
        }

        fullBinary[startPosSet] = '\0';
        strcpy(tag, fullBinary);

        LRU curLRU = lru[setVal];
        cacheSimulateOne(&curLRU, tag, commandType, hits, misses, evicitons);
    }

}

int main(int argc, char** argv)
{

    int s;
    int e;
    int b;
    char fileName[100];

    // step1: parse option
    parseOption(&s, &e, &b, argc, argv, &fileName);

    // step2: read all of the lines and analyze it
    int hits = 0;
    int misses = 0;
    int evictions = 0;
    cacheSimulateWhole(fileName, &hits, &misses, &evictions, s, b);
    printSummary(hits, misses, evictions);
    return 0;
}
