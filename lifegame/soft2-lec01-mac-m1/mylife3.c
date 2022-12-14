#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // sleep()関数を使う
#include <time.h>
#include "gol3.h"

int main(int argc, char **argv) {
    FILE *fp = stdout;
    const int height = 40;
    const int width = 70;

    int cell[height][width];
    for(int y = 0 ; y < height ; y++){
        for(int x = 0 ; x < width ; x++){
            cell[y][x] = 0;
        }
    }

    /* ファイルを引数にとるか、ない場合はデフォルトの初期値を使う */
    if ( argc > 2 ) {
        fprintf(stderr, "usage: %s [filename for init]\n", argv[0]);
        return EXIT_FAILURE;
    }

    // ここの中を,lifとrleで分岐させる
    else if (argc == 2) {
        FILE *lgfile;
        if ( (lgfile = fopen(argv[1],"r")) != NULL ) {
            init_cells_3(height, width, cell, lgfile);
        }
        else {
            fprintf(stderr,"cannot open file %s\n",argv[1]);
            return EXIT_FAILURE;
        }
    }
    //

    else{
        srand((unsigned)time(NULL));
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                  double x = (double)rand() / RAND_MAX;
                  if (x > 0.9) {
                    cell[i][j] = 1;
                  } else {
                    cell[i][j] = 0;
                  }
            }
        }
    }
    
    print_cells_3(fp, 0, height, width, cell); // 表示する
    sleep(1); // 1秒休止
    fprintf(fp,"\e[%dA",height+3);//height+3 の分、カーソルを上に戻す(壁2、表示部1)
    
    /* 世代を進める*/
    for (int gen = 1 ;; gen++) {
        update_cells_3(height, width, cell); // セルを更新
        print_cells_3(fp, gen, height, width, cell);  // 表示する
        sleep(1); //1秒休止する
        fprintf(fp,"\e[%dA",height+3);//height+3 の分、カーソルを上に戻す(壁2、表示部1)

        if (gen % 100 == 0 && gen < 1000) {
            // ファイル名の調整
            char name[11] = "gen0000.lif";
            char number[10] = "0123456789";
            for (int i = 0; i < 10; i++) {
                if (gen / 1000 == i) {
                name[3] = number[i];
                }
                if ((gen % 1000) / 100 == i) {
                name[4] = number[i];
                }
            }
            // データの書き込み
            FILE *fq = fopen(name, "a+"); // 既存なら追加、未開ならファイルを作成
            fprintf(fq, "#Life 1.06\n");
            for (int i = 0; i < 40; i++) {
                for (int j = 0; j < 70; j++) {
                    if (cell[i][j] == 1) {
                        fprintf(fq, "%d %d\n", j, i);
                    }
                }
            } 
            fclose(fq);
        }
        
    }
    
    return EXIT_SUCCESS;
}
