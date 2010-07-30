#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    FILE *fh;
    unsigned char byte;
    long i;
    long size;

    if(argc < 2) {
        fprintf(stderr, "Usage: %s <binary> [array name]\n", argv[0]);
        exit(1);
    }

    fh = fopen(argv[1], "r");
    if(!fh) {
        fprintf(stderr, "%s: ", argv[1]);
        perror(NULL);
        exit(1);
    }

    fseek(fh, 0, SEEK_END);
    size = ftell(fh);
    fseek(fh, 0, SEEK_SET);

    printf("unsigned char %s[%ld] = {\n\t", argc > 2 ? argv[2] : "bin2hex", size);

    for(i = 0; i < size; i++) {
        byte = (unsigned char)fgetc(fh);
        printf("0x%02X", byte);

        if((i+1) < size) {
            printf(", ");
            if((i+1) % 8 == 0)
                printf("\n\t");
        }
    }

    printf("\n};\n");

    return 0;
}
