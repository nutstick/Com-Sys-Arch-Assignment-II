#include <bits/stdc++.h>

#define TAGLEN 22
#define CACHE_SIZE 4096
#define K_WayS 4

long MISS;
long HIT;
int INDEX_SIZE;
int INDEXLEN;
int OFFSETLEN;

typedef unsigned char Byte;
struct Cache {
    bool valid;
    unsigned long tag;
    unsigned long last;
};

struct Cache cache[50000][10];
unsigned long last;
Byte Way;

void init() {
    MISS    = 0;
    HIT     = 0;
    for ( int i=0; i<CACHE_SIZE; i++ ) {
        for ( int j=0; j<K_WayS; j++ ) {
            cache[ i ][ j ].valid   = false;
            cache[ i ][ j ].tag     = 0;
            cache[ i ][ j ].last    = 0;
        }
    }
}

void calAddr(unsigned long addr,unsigned long *tag,unsigned long *idx,unsigned long *offset) {
    unsigned long tmp;
    int i;
    *tag=addr>>(OFFSETLEN+INDEXLEN);
    tmp=0;
    for(i=0; i<INDEXLEN; i++) {
        tmp<<=1;
        tmp|=0x01;
    }
    *idx=addr>>OFFSETLEN & tmp;
    tmp=0;
    for(i=0; i<OFFSETLEN; i++) {
        tmp<<=1;
        tmp|=0x01;
    }
    *offset=addr & tmp;
}
void access(unsigned long addr) {
    unsigned long offset;
    unsigned long tag;
    unsigned long idx;
    calAddr( addr, &tag, &idx, &offset );

    int foudingIndex = -1;
    int avaliableSlot = -1;
    for ( int i=0; i<Way; i++ ) {
        if (!cache[ idx ][ i ].valid) {
            avaliableSlot = i;
        }

        if ( cache[ idx ][ i ].tag == tag && cache[ idx ][ i ].valid ) {
            foudingIndex = i;
            break;
        }
    }

    if ( foudingIndex != -1 ) {
        HIT++;
        cache[ idx ][ foudingIndex ].last = last;
    } else {
        MISS++;
        if ( avaliableSlot == -1 ) {
            unsigned long _min = 0x7ffffff;
            for ( int i=0; i < Way; i++ ) {
                if ( _min > cache[idx][i].last ) {
                    _min = cache[idx][i].last;
                    avaliableSlot = i;
                }
            }
        }
        cache[ idx ][ avaliableSlot ].valid = 1;
        cache[ idx ][ avaliableSlot ].tag   = tag;
        cache[ idx ][ avaliableSlot ].last  = last;
    }
    last++;

}
int main(int argc,char *argv[]) {
    freopen( "LRU_out.txt", "w", stdout );

    if (argc<3) {
        fprintf( stderr, "Please specify address two file\n" );
        return -1;
    }

    int cachesize[] = { 1, 4, 8, 32, 512, 1024 };
    int cachebit[]  = { 0, 2, 4, 5, 9, 10 };
    char buff[1025];

    for ( int file=1; file<=2; file++ ) {

        printf( "miss rate of file: %s\n", argv[file] );
        printf( " \tLRU-2Way\tLRU-4Way\n" );

        for ( int i=0; i<6; i++ ) {

            printf("%d",cachesize[i]);

            for ( Way = 2; Way <= K_WayS; Way+=2 ) {

                INDEX_SIZE = cachesize[i]*1024/Way/32;
                INDEXLEN = 10+cachebit[i]-(Way/2)-5;
                OFFSETLEN = 5;

                FILE *input = fopen( argv[file],"r" );
                unsigned int myaddr;

                init();

                while ( fgets( &buff[0], 1024, input ) ) {
                    sscanf( buff, "0x%x", &myaddr );
                    access( myaddr );
                }
                printf("\t%f",(double)(MISS)/(HIT+MISS)*100);
            }
            printf("\n");
        }
        printf("\n");
    }
}
