
// ↓関数の名前を変えて運用して実験
void init_cells_2(const int height, const int width, int cell[height][width], FILE* fp) {
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

// グリッドの描画: 世代情報とグリッドの配列等を受け取り、ファイルポインタに該当する出力にグリッドを描画する
void print_cells_2(FILE *fp, int gen, const int height, const int width, int cell[height][width]) {
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

void update_cells_2(const int height, const int width, int cell[height][width]) {
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