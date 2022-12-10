/*
 ファイルによるセルの初期化: 生きているセルの座標が記述されたファイルをもとに2次元配列の状態を初期化する
 fp = NULL のときは、関数内で適宜定められた初期状態に初期化する。関数内初期値はdefault.lif と同じもの
 */
void init_cells(const int height, const int width, int cell[height][width], FILE* fp) {
  if (fp == NULL) {
    cell[20][30] = 1;
    cell[22][30] = 1;
    cell[22][31] = 1;
    cell[23][31] = 1;
    cell[20][32] = 1;
  } else {
    int x, y;
    char s[10];
    fscanf(fp, "%s", s);
    fscanf(fp, "%s", s); // 最初の2行?が余計な文字列
    while (fscanf(fp, "%d %d", &x, &y) != EOF) {
      printf("%d, %d\n", x, y);
      cell[y][x] = 1;
    }
  }
  //close the file.
  fclose(fp);
}

/*
 グリッドの描画: 世代情報とグリッドの配列等を受け取り、ファイルポインタに該当する出力にグリッドを描画する
 */
void print_cells(FILE *fp, int gen, const int height, const int width, int cell[height][width]) {
  system("clear"); // コンソールのクリア
  printf("generation = %d\r\n", gen);
  printf("+");
  for (int i = 0; i < 70; i++) {
    printf("-");
  }
  printf("+\r\n");

  for (int i = 0; i < 40; i++) {
    printf("|");
    for (int j = 0; j < 70; j++) {
      if (cell[i][j] == 0) {
        printf(" ");
      } else {
        printf("\x1b[31m#\x1b[0m");
      }
    }
    printf("|\r\n");
  }

  printf("+");
  for (int i = 0; i < 70; i++) {
    printf("-");
  }
  printf("+\r\n");
}

/*
 着目するセルの周辺の生きたセルをカウントする関数
 */
int count_adjacent_cells(int h, int w, const int height, const int width, int cell[height][width]);

/*
 ライフゲームのルールに基づいて2次元配列の状態を更新する
 */
void update_cells(const int height, const int width, int cell[height][width]) {
  int cell_expanded[height + 2][width + 2];
  // 初期配置設定
  for (int i = 0; i < height + 2; i++) {
      for (int j = 0; j < width + 2; j++) {
        cell_expanded[i][j] = 0;
      }
  }
  for (int i = 1; i < height + 1; i++) {
      for (int j = 1; j < width + 1; j++) {
        cell_expanded[i][j] = cell[i - 1][j - 1];
      }
  }
  // 世代交代
  int count;
  for (int i = 1; i < height + 1; i++) {
      for (int j = 1; j < width + 1; j++) {
        count = 0;
        count += cell_expanded[i - 1][j - 1];
        count += cell_expanded[i - 1][j];
        count += cell_expanded[i - 1][j + 1];
        count += cell_expanded[i][j - 1];
        count += cell_expanded[i][j + 1];
        count += cell_expanded[i + 1][j - 1];
        count += cell_expanded[i + 1][j];
        count += cell_expanded[i + 1][j + 1];
        // 生きてるセルについて
        if (cell_expanded[i][j] == 1) {
          if (count != 2 && count != 3) {
            cell[i - 1][j - 1] = 0;
          }
        }
        // 空のセルについて
        else {
          if (count == 3) {
            cell[i - 1][j - 1] = 1;
          }
        }
      }
  }
}