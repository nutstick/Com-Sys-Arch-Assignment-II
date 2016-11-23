#include <stdio.h>
#include <string.h>

#define TAGLEN 22
#define BLOCK_SIZE 4
#define CACHE_SIZE 4096

int INDEX_SIZE;
int INDEXLEN;
int OFFSETLEN;
long MISS;
long HIT;

typedef unsigned char Byte;
typedef unsigned char bool;
typedef struct Cache {
    bool valid;
    bool dirty;
    unsigned long tag;
    Byte data[BLOCK_SIZE];
};


int init(struct Cache *cache) {
    MISS=0;
    HIT=0;
    int i;
    for(i=0; i<INDEX_SIZE; i++) {
        cache[i].valid=0;
        cache[i].tag=0;
        cache[i].dirty=0;
    }
}
int calAddr(unsigned long addr,unsigned long *tag,unsigned long *idx,unsigned long *offset) {
    unsigned long tmp;
    int i;

    *tag=addr>>(OFFSETLEN+INDEXLEN); // get tag from address have ALLBIT - INDEXLEN - OFFSETLEN bit


    tmp=0;
    for(i=0; i<INDEXLEN; i++) {
        tmp<<=1;
        tmp|=0x01;
    }
    *idx=addr>>OFFSETLEN & tmp; // get idx from address  have INDEXLEN Bit


    tmp=0;
    for(i=0; i<OFFSETLEN; i++) {
        tmp<<=1;
        tmp|=0x01;
    }
    *offset=addr & tmp;  // get offset from address  have OFFSETLEN Bit
}
int access(struct Cache *cache, unsigned long addr) {
    unsigned long offset;
    unsigned long tag;
    unsigned long idx;
    int i;
    int valid=0;
    calAddr(addr,&tag,&idx,&offset);
    if ( cache[idx].tag == tag && cache[idx].valid ) {
        HIT++;
    } else {
        MISS++;
        cache[ idx ].valid = true;
        cache[ idx ].tag = tag;
    }

}
int main(int argc,char *argv[]) {
    if (argc<3) {
        fprintf(stderr,"Please specify address two file\n");
        exit(-1);
    }

    freopen("output.txt","w",stdout);

    int file;
    for(file=1; file<=2; file++) {
        int i;
        printf("miss rate of file: %s\n",argv[file]);
        printf(" \t4\t\t8\t\t16\t\t32\n");
        for(i=2; i<=5; i++) {
            int j;
            printf("%d",1<<i);
            for(j=2; j<=5; j++) {
                INDEX_SIZE = (1<<j)*1024/(1<<i);
                INDEXLEN = 10+j-i;
                OFFSETLEN = i;
                //            printf("%d %d %d\n",INDEX_SIZE,INDEXLEN,OFFSETLEN);

                FILE *input = fopen(argv[file],"r");
                char buff[1025];
                unsigned long myaddr;

                struct Cache cache[INDEX_SIZE];

                init(cache);

                while (fgets(& buff[0],1024,input)) {

                    sscanf(buff,"0x%x",&myaddr);
                    access(cache,myaddr);
                }
                //printf("HIT:%7d MISS: %7d\n",HIT,MISS);
                printf("\t%lf",(double)(MISS)/(HIT+MISS) * 100);

            }
            printf("\n");
        }
        printf("\n");
    }
}
