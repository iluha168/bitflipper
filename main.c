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
    int verbose_flag = 0;
    int implementation = 0;
    unsigned long headerSize = 0;
    char* buffer;

    const struct option long_options[] = {
    /*0*/{"verbose", no_argument, &verbose_flag, 1},
    /*1*/{"help"   , no_argument, NULL, 'h'},
    /*2*/{"seed"   , required_argument, NULL, 's'},
    /*3*/{"amount" , required_argument, NULL, 'n'},
    /*4*/{"disk"   , no_argument, &implementation, 0},
    /*5*/{"ram"    , no_argument, &implementation, 1},
    /*6*/{"skip"   , required_argument, NULL, 0},
    /*7*/{0, 0, NULL, 0}
    };

    int long_option_i = 0;
    char res;
    while((res = getopt_long(argc, argv, "hn:vs:", long_options, &long_option_i)) != -1){
        switch (res){
        case 0:
            if(long_options[long_option_i].flag != NULL) break;
            switch(long_option_i){
                case 6: //skip
                    headerSize = strtoul(optarg, NULL, 10);
                break;
            }
        break;
        case 'n':
            toflip = strtoul(optarg, NULL, 10);
        break;
        case 'v':
            verbose_flag = 1;
        break;
        case 's':
            if(strcmp(optarg, "time") == 0)
                srandl(time(NULL));
            else srandl(strtoul(optarg, NULL, 10));
        break;
        case '?':
        case 'h':
            printf("Usage:\nbitflipper [OPTION]… [FILENAME]\n"
                "\t-h, --help\tshow this message and exit(0)\n"
                "\t-v, --verbose\tenables verbose mode\n"
                "\t-s, --seed\tset seed to generate positions of bits to flip (default: 0)\n"
                "\t-s, --seed time\tset seed from current system time\n"
                "\t-n, --amount\tset amount of bits to flip in a file (default: 0)\n"
                "\t--disk\twrite changes directly into the file (best for low --amount)\n"
                "\t--ram\tmake changes in the memory, then rewrite the file (best for high --amount)\n"
                "\t--skip\t set header size - amount of bytes from the beginning of the file that will not be affected.\n"
            );
        exit(0);
        }
        if(errno == ERANGE) exit(-1);
    }

    const char* filename = argv[optind];
    if(verbose_flag){
        printf("Bits to flip: %d\n", toflip);
        printf("File to flip: %s\n", filename);
        printf("Seed: %lu\n", seed);
        printf("Header size: %d\n", headerSize);
    }
    //actual code
    FILE *f = fopen(filename, "r+");
    if(f == NULL) exit(-2);
    fseek(f, 0, SEEK_END);
    const long fsizeWOHeader = ftell(f)-headerSize;
    if(implementation == 1){
        buffer = malloc(fsizeWOHeader);
        fseek(f, headerSize, SEEK_SET);
        for(size_t read = 0; read < fsizeWOHeader; read += fread(buffer+read, 1, fsizeWOHeader-read, f));
    }
    while(toflip --> 0){
        long pos = randl()%fsizeWOHeader;
        const char bitmask = 1 << (rand()%8);
        if(implementation == 0){
            fseek(f, pos+headerSize, SEEK_SET);
            char byte;
            fread(&byte, 1, 1, f);
            byte ^= bitmask;
            fseek(f, pos+headerSize, SEEK_SET);
            fwrite(&byte, 1, 1, f);
        } else if(implementation == 1){
            buffer[pos] ^= bitmask;
        }
        if(verbose_flag) printf("Flipping... Left: %d        \r", toflip);
    }
    if(implementation == 1){
        fseek(f, headerSize, SEEK_SET);
        for(size_t written = 0; written < fsizeWOHeader; written += fwrite(buffer+written, 1, fsizeWOHeader-written, f));
        free(buffer);
    }
    fclose(f);
}