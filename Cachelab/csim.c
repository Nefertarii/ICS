#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct Argvalue
{
    int h;         //print usage info
    int v;         //displays trace info
    int s;         //number of set index
    int E;         //number of lines per set associativity
    int b;         //number of block bits
    const char *t; //name of the valgrind trace to replay
};
struct Cacheblock
{
    int valid;        //有效位
    unsigned int Tag; //标记位
    int lru;          //lru标记
    int B;            //高速缓存块地址长度
};
struct Cache
{
    int S;         //总共多少组
    int E;         //每组多少行
    int blocksize; //总占用大小
};
struct Hitsum
{
    int hits;
    int misses;
    int evictions;
};

typedef struct Argvalue *Args;
typedef struct Cacheblock *CacheLine;
typedef struct Cache *CacheSet;
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
    return 0;
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
void initcache(int s, int E, int b, CacheLine cacheline, CacheSet cacheset)
{
    int i, tmp = 2;
    for (i = 1; i != s; i++)
    {
        tmp = tmp * 2;
    }
    cacheset->S = tmp;
    tmp = 2;
    cacheset->E = E;
    for (i = 1; i != b; i++)
    {
        tmp = tmp * 2;
    }
    cacheline->B = tmp;
    tmp = 2;
    cacheset->blocksize = cacheline->B * cacheset->E * cacheset->S;

    cacheline->lru = 0;
    cacheline->Tag = 0;
    cacheline->valid = 0;
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
    struct Cacheblock cacheline;
    struct Cache cacheset;
    struct Hitsum cachehits;
    initval(&argvalue, &cachehits);
    if (loadargv(argc, argv, &argvalue) == -1)
    {
        errorinput();
        exit(1);
    }
    initcache(argvalue.s, argvalue.E, argvalue.b, &cacheline, &cacheset);
    int Cachesets[cacheset.S];
    int Setblocks[cacheset.E];
    int Blockbits[cacheline.B];

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
