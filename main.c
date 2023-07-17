#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <time.h>

unsigned long seed;
void srandl(unsigned long newSeed){
    seed = newSeed;
    srand(seed % __UINT32_MAX__);
}

long randl(){
    union {
        long l;
        int i[sizeof(long)/sizeof(int)];
    } randval;
    for(unsigned i = 0; i < sizeof(long)/sizeof(int); i++)
        randval.i[i] = rand();
    return randval.l;
}

int main(int argc, char** argv){
    int toflip = 0;
    unsigned char verbose = 0;

    char res;
    while((res = getopt(argc, argv, "hn:vs:")) != -1){
        switch (res){
        case 'n':
            toflip = strtoul(optarg, NULL, 10);
        break;
        case 'v':
            verbose = 1;
        break;
        case 's':
            if(strcmp(optarg, "time") == 0)
                srandl(time(NULL));
            else srandl(strtoul(optarg, NULL, 10));
        break;
        case 'h':
            printf("Usage:\nbitflipper [-n N] [-v] [-s SEED] [-h]\n"
                "\t-h\tshow this message and exit(0)\n"
                "\t-v\tenables verbose mode\n"
                "\t-s\tset seed to generate positions of bits to flip (default: 0)\n"
                "\t-stime\tset seed from current system time\n"
                "\t-n\tset amount of bits to flip in a file (default: 0)\n"
            );
        exit(0);
        }
        if(errno == ERANGE) exit(-1);
    }

    const char* filename = argv[optind];
    if(verbose){
        printf("Bits to flip: %d\n", toflip);
        printf("File to flip: %s\n", filename);
        printf("Seed: %lu\n", seed);
    }
    //actual code
    FILE *f = fopen(filename, "r+");
    if(f == NULL) exit(-2);
    fseek(f, 0, SEEK_END);
    long filesize = ftell(f);
    while(toflip --> 0){
        long pos = randl()%filesize;
        fseek(f, pos, SEEK_SET);
        char byte;
        fread(&byte, 1, 1, f);
        byte = byte ^ (1 << (rand()%8));
        fseek(f, pos, SEEK_SET);
        fwrite(&byte, 1, 1, f);
        if(verbose) printf("Flipping... Left: %d        \r", toflip);
    }
    fclose(f);
}