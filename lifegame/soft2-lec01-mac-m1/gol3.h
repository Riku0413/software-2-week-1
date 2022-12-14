// 課題3のためのファイル

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
    char s[3000];
    fscanf(fp, "%[^\n]", s); // 最初の\nまでをスキャン

    // #Life 1.06 の場合
    if (s[1] == 'L' && s[2] == 'i') { // 十分条件

      while (fscanf(fp, "%d %d", &x, &y) != EOF) {
        cell[y][x] = 1;
      }
    }
    
    // RLE の場合
    else {
      // 説明文が終わるまでスキャン
      if (s[0] != 'x') {
        while(1) {
          fscanf(fp, "%[^\n]", s);
          if (s[0] == 'x') {
            break;
          }
        }
      }

      // 本文の読み込み
      int line = 0; // 何行目のセルかを指定
      int raw = 0; // 何列目のセルかを指定

      // 一旦全部読み込み
      fscanf(fp, "%[^!]", s);


      char data[100];
      int p_s = 0;
      int p_data = 0;

      while (1) {
        if (s[p_s] == '\n') {
          p_s++;
          continue;
        }

        data[p_data] = s[p_s];

        if (s[p_s] == '$' | s[p_s] == '!') {
          // ここで1行のセル更新
          int i = 0; // ここ
          char buff[3]; // \0の枠も確保!
          buff[0] = '0'; buff[1] = '0'; buff[2] = '\0';// デフォルトでは文字の0 -> bobo みたいなみたいな符号にも対応！！
          int rank = 0;
          while (data[i] != '$' && data[i] != '!') { // 1行が終わるまでのループ
            if (data[i] >= '0' && data[i] <= '9') {
              buff[rank] = data[i];
              rank++;
            }
            else { // b か o のとき
              buff[rank] = '\0';
              int num = atoi(buff); // ここのincludeは問題ない
              if (num == 0) {
                num = 1;
              }
              for (int k = 0; k < num; k++) {
                if (data[i] == 'b') {
                  cell[line][raw] = 0; // ここ
                }
                if (data[i] == 'o') {
                  cell[line][raw] = 1; // ここ
                }
                raw++;
              }
              rank = 0;
              buff[0] = '0'; buff[1] = '0';
            }
            i++;
          }

          raw = 0;
          line++;



          p_data = 0; // リセット
          if (s[p_s] == '$') {
            p_s++;
            continue; // 次の行へ
          } else { // '!'のとき
            break;
          }
        }

        p_data++;
        p_s++;

      }
      p_s = 0;
    }
  }
  //close the file.
  fclose(fp);
}

// グリッドの描画: 世代情報とグリッドの配列等を受け取り、ファイルポインタに該当する出力にグリッドを描画する
void print_cells_2(FILE *fp, int gen, const int height, const int width, int cell[height][width]) {
  // 比率の調査
  double ratio = 0;
  for (int i = 0; i < height; i++) {
      for (int j = 0; j < width; j++) {
        if (cell[i][j] == 1) {
          ratio += 1;
        }
      }
  }
  ratio /= (height * width);
  //
  system("clear"); // コンソールのクリア
  printf("generation = %d, ratio = %lf\r\n", gen, ratio);
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