#include <stdio.h>
#include <stdlib.h>
#include "memo.h"

int main(int argc, char **argv) {
    if ( argc > 2 ) {
        fprintf(stderr, "usage: %s [filename for init]\n", argv[0]);
        return EXIT_FAILURE;
    }
    else if (argc == 2) {
        FILE *lgfile;
        if ( (lgfile = fopen(argv[1],"r")) != NULL ) {
            init_cells(lgfile); // ファイルによる初期化
        }
        else{
            fprintf(stderr,"cannot open file %s\n",argv[1]);
            return EXIT_FAILURE;
        }
        fclose(lgfile);
    }
}