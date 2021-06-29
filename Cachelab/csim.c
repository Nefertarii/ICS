#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct Argvalue
{
    int h; //print usage info
    int v; //displays trace info
    int s; //number of set index
    int E; //number of lines per set associativity
    int b; //number of block bits
    int S;
    int B;
    int blocksize;
    const char *t; //name of the valgrind trace to replay
};
struct Cacheblock
{
    int valid;        //有效位
    unsigned int tag; //标记位
    int lru;          //lru标记
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
void initval(Args argvalue, CacheHits cachehits)
{
    argvalue->h = -1;
    argvalue->v = -1;
    argvalue->s = -1;
    argvalue->E = -1;
    argvalue->b = -1;
    argvalue->t = NULL;

    cachehits->evictions = 0;
    cachehits->misses = 0;
    cachehits->hits = 0;
}
int loadargv(int argc, const char *argv[], Args argvalue)
{
    int i, tmp;
    for (i = 1; i != argc; i++)
    {
        if (strcmp(argv[i], "-h") == 0)
        {
            argvalue->h = 0;
        }
        else if (strcmp(argv[i], "-v") == 0)
        {
            argvalue->v = 0;
        }
        else if (strcmp(argv[i], "-s") == 0)
        {
            tmp = atoi(argv[++i]);
            argvalue->s = tmp;
        }
        else if (strcmp(argv[i], "-E") == 0)
        {
            tmp = atoi(argv[++i]);
            argvalue->E = tmp;
        }
        else if (strcmp(argv[i], "-b") == 0)
        {
            tmp = atoi(argv[++i]);
            argvalue->b = tmp;
        }
        else if (strcmp(argv[i], "-t") == 0)
        {
            argvalue->t = argv[++i];
        }
        else
        {
            return -1;
        }
    }
    if (argvalue->s <= 0 || argvalue->E <= 0 || argvalue->b <= 0 || argvalue->t == NULL)
        return -1;
    tmp = 2;
    for (i = 1; i != argvalue->s; i++)
    {
        tmp = tmp * 2;
    }
    argvalue->S = tmp;
    tmp = 2;
    for (i = 1; i != argvalue->b; i++)
    {
        tmp = tmp * 2;
    }
    argvalue->B = tmp;
    argvalue->blocksize = argvalue->B * argvalue->E * argvalue->S;
    return 0;
}

void initcache(Args argvalue, CacheSet cacheset)
{
    //多维数组的开辟要一行行malloc
    cacheset = (CacheSet)malloc(sizeof(CacheLine) * argvalue->S);
    for (int i = 0; i < argvalue->S; ++i)
    {
        cacheset[i] = (CacheLine)malloc(sizeof(struct Cacheblock) * argvalue->E);
        for (int j = 0; j < argvalue->E; ++j)
        {
            cacheset[i][j].valid = 0;
            cacheset[i][j].tag = -1;
            cacheset[i][j].lru = -1;
        }
    }
}
int transfile(const char *filename)
{
    FILE *fp = fopen(filename, "r");
    if (fp == NULL)
    {
        printf("File open error\n");
        return -1;
    }
    fclose(fp);
    return 0;
}
void simulate();
void printSummary(int hits, int misses, int evictions)
{
    printf("Hits:%d   Misses:%d   evictions:%d", hits, misses, evictions);
}
int main(int argc, const char *argv[])
{
    if (argc > 10 || argc == 1)
    {
        errorinput();
        return 0;
    }
    struct Argvalue argvalue;
    struct Hitsum cachehits;
    CacheSet cacheset = NULL;
    initval(&argvalue, &cachehits);
    if (loadargv(argc, argv, &argvalue) == -1)
    {
        errorinput();
        exit(1);
    }
    initcache(&argvalue, cacheset);

    /*
    if (transfile(argvalue.t) == -1)
    {
        printf("invalid file\n");
        exit(1);
    }
    */
    //simulate();
    printSummary(cachehits.hits, cachehits.misses, cachehits.evictions);
    return 0;
}
