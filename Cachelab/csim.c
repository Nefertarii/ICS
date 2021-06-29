#include "cachelab.h"
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

//void printSummary(int hits, int misses, int evictions) { printf("hits:%d misses:%d evictions:%d", hits, misses, evictions); }

struct Argvalue
{
    int h;         //print usage info
    int v;         //displays trace info
    int s;         //number of set index
    int E;         //number of lines per set associativity
    int b;         //number of block bits
    int S;         //2^s number of set
    int B;         //2^b number of block size
    int blocksize; //m total block size
    const char *t; //name of the valgrind trace to replay
};
struct Cacheblock
{
    int valid;        //有效位
    unsigned int tag; //标记位
    int lru;          //lru标记,次数
};
struct Hitsum
{
    int hits;
    int misses;
    int evictions;
};

typedef struct Argvalue *Args;
typedef struct Cacheblock *CacheLine;
typedef struct Cacheblock **CacheSet;
typedef struct Hitsum *CacheHits;

struct Argvalue argvalue;
struct Hitsum cachehits;
CacheSet cacheset = NULL;

void errorinput()
{
    printf("./csim: Missing required command line argument\n"
           "Usage: ./csim [-hv] -s <num> -E <num> -b <num> -t <file>\n"
           "Options:\n"
           "-h         Print this help message.\n"
           "-v         Optional verbose flag.\n"
           "-s <num>   Number of set index bits.\n"
           "-E <num>   Number of lines per set.\n"
           "-b <num>   Number of block offset bits.\n"
           "-t <file>  Trace file.\n\n"
           "Examples:\n"
           "linux>  ./csim -s 4 -E 1 -b 4 -t traces/yi.trace\n"
           "linux>  ./csim -v -s 8 -E 2 -b 4 -t traces/yi.trace\n");
}
void initval()
{
    argvalue.h = -1;
    argvalue.v = -1;
    argvalue.s = -1;
    argvalue.E = -1;
    argvalue.b = -1;
    argvalue.t = NULL;

    cachehits.evictions = 0;
    cachehits.misses = 0;
    cachehits.hits = 0;
}
int loadargv(int argc, const char *argv[])
{
    int i, tmp;
    for (i = 1; i != argc; i++)
    {
        if (strcmp(argv[i], "-h") == 0)
        {
            argvalue.h = 0;
        }
        else if (strcmp(argv[i], "-v") == 0)
        {
            argvalue.v = 0;
        }
        else if (strcmp(argv[i], "-s") == 0)
        {
            tmp = atoi(argv[++i]);
            argvalue.s = tmp;
        }
        else if (strcmp(argv[i], "-E") == 0)
        {
            tmp = atoi(argv[++i]);
            argvalue.E = tmp;
        }
        else if (strcmp(argv[i], "-b") == 0)
        {
            tmp = atoi(argv[++i]);
            argvalue.b = tmp;
        }
        else if (strcmp(argv[i], "-t") == 0)
        {
            argvalue.t = argv[++i];
        }
        else
        {
            return -1;
        }
    }
    if (argvalue.s <= 0 || argvalue.E <= 0 || argvalue.b <= 0 || argvalue.t == NULL)
        return -1;
    tmp = 2;
    for (i = 1; i != argvalue.s; i++)
    {
        tmp = tmp * 2;
    }
    argvalue.S = tmp;
    tmp = 2;
    for (i = 1; i != argvalue.b; i++)
    {
        tmp = tmp * 2;
    }
    argvalue.B = tmp;
    argvalue.blocksize = argvalue.B * argvalue.E * argvalue.S;
    return 0;
}
void initcache()
{
    //多维数组的开辟要一行行malloc
    cacheset = (CacheSet)malloc(sizeof(CacheLine) * argvalue.S);
    for (int i = 0; i < argvalue.S; ++i)
    {
        cacheset[i] = (CacheLine)malloc(sizeof(struct Cacheblock) * argvalue.E);
        for (int j = 0; j < argvalue.E; ++j)
        {
            cacheset[i][j].valid = 0;
            cacheset[i][j].tag = -1;
            cacheset[i][j].lru = -1;
        }
    }
}

//return 0 = hit 1 = miss -1 = eviction
void CacheOP(unsigned int address, int memorysize)
{
    int i, index, tag, maxLRU = 0;
    int E = argvalue.E;
    //知道为什么 但是这里没想到这个index 和 tag运算
    index = (address >> argvalue.B) & ((-1U) >> (64 - argvalue.s));
    tag = address >> (argvalue.b + argvalue.s);
    for (i = 0; i != E; i++)
    {
        //先判断是否能命中 成功命中
        if (cacheset[index][i].tag == tag)
        {
            cachehits.hits++;
            cacheset[index][i].lru = 0;
            return;
        }
        //命中不了查看是否有剩余空块 直接插入
        if (cacheset[index][i].valid == 0)
        {
            cacheset[index][i].valid = 1;
            cacheset[index][i].tag = tag;
            cacheset[index][i].lru = 0;
            cachehits.misses++;
            return;
        }
    }
    //执行至此 既无空行也没用命中 进行LRU替换
    cachehits.evictions++;
    cachehits.misses++;
    for (i = 1; i != E - 1; i++)
    {
        if (cacheset[index][i - 1].lru >= cacheset[index][i].lru)
            maxLRU = i - 1;
        else
            maxLRU = i;
    }
    cacheset[index][maxLRU].valid = 1;
    cacheset[index][maxLRU].tag = tag;
    cacheset[index][maxLRU].lru = 0;
    return;
}
int simulate(const char *filename)
{
    char operation; //L = load  S = store  M = modfiy
    unsigned int address;
    int memorysize;
    FILE *fp = fopen(filename, "r");
    if (fp == NULL)
    {
        printf("File open error\n");
        return -1;
    }
    while (fscanf(fp, " %c %xu,%d\n", &operation, &address, &memorysize) > 0)
    {
        switch (operation)
        {
        case 'L':
            CacheOP(address, memorysize);
            break;
        case 'S':
            CacheOP(address, memorysize);
            break;
        case 'M':
            CacheOP(address, memorysize);
            break;
        default:
            break;
        }
    }
    fclose(fp);
    return 0;
}

int main(int argc, const char *argv[])
{
    if (argc > 10 || argc == 1)
    {
        errorinput();
        return 0;
    }
    
    initval();
    if (loadargv(argc, argv) == -1)
    {
        errorinput();
        exit(1);
    }
    initcache();
    if (simulate(argvalue.t) == -1)
    {
        printf("simulate error!\n");
        exit(1);
    }
    printSummary(cachehits.hits, cachehits.misses, cachehits.evictions);
    free(cacheset);
    return 0;
}
