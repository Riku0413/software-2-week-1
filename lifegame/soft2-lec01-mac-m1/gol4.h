// 課題3のためのファイル

void init_cells_4(const int height, const int width, int cell[height][width], FILE* fp) {

  // 乱数の準備
  srand((unsigned)time(NULL));
  double r1 = (double)rand() / RAND_MAX;
  double r2 = (double)rand() / RAND_MAX;
  
  // ランダムな初期化
  if (fp == NULL) {
    for (int i = 0; i < height; i++) {
      for (int j = 0; j < width; j++) {
        r1 = (double)rand() / RAND_MAX;
        r2 = (double)rand() / RAND_MAX;
          if (r1 > 0.9) {
            if (r2 > 0.5) {
              cell[i][j] = 1;
            } else {
              cell[i][j] = 10;
            }
          }
      }
    }
  }
  // ファイルを指定して初期化
  else {
    int x, y;
    char s[3000];
    fscanf(fp, "%[^\n]", s); // 最初の\nまでをスキャン

    // #Life 1.06 の場合
    if (s[1] == 'L' && s[2] == 'i') { // 十分条件

      while (fscanf(fp, "%d %d", &x, &y) != EOF) {
        r1 = (double)rand() / RAND_MAX;
        if (r1 > 0.5) {
          cell[y][x] = 1; // 上半分は 赤 勢力
        } else {
          cell[y][x] = 10; // 下半分は 青 勢力
        }
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

      // 一旦全部読み込み
      fscanf(fp, "%[^!]", s);

      char data[100];
      int p_s = 0;
      int p_data = 0;
      int line = 0; // 何行目のセルかを指定
      int raw = 0; // 何列目のセルかを指定

      while (1) {
        if (s[p_s] == '\n' | s[p_s] == ' ') {
          p_s++;
          continue;
        }

        data[p_data] = s[p_s];

        if (s[p_s] == '$' | s[p_s] == '!') {
          // ここで1行のセル更新
          int i = 0;
          char buff[3]; // \0の枠も確保!
          buff[0] = '0'; buff[1] = '0'; buff[2] = '\0';// デフォルトでは文字の0 -> bobo みたいな符号にも対応！！
          int rank = 0;
          while (data[i] != '$' && data[i] != '!') { // 1行が終わるまでのループ
            if (data[i] >= '0' && data[i] <= '9') {
              buff[rank] = data[i];
              rank++;
            }
            else { // b か o のとき
              buff[rank] = '\0';
              int num = atoi(buff);
              if (num == 0) {
                num = 1;
              }
              for (int k = 0; k < num; k++) {
                // if (data[i] == 'b') {
                //   cell[line][raw] = 0;
                // }
                if (data[i] == 'o') {
                  r1 = (double)rand() / RAND_MAX;
                  if (r1 > 0.5) {
                    cell[line][raw] = 1; // 赤
                  } else {
                    cell[line][raw] = 10; // 青
                  }
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
  fclose(fp);
}

// グリッドの描画: 世代情報とグリッドの配列等を受け取り、ファイルポインタに該当する出力にグリッドを描画する
void print_cells_4(FILE *fp, int gen, const int height, const int width, int cell[height][width], int number[3]) {

  // 残兵数の調査
  int red = 0;
  int blue = 0;
  for (int i = 0; i < height; i++) {
      for (int j = 0; j < width; j++) {
        if (cell[i][j] == 1) {
          red += 1;
        } else if (cell[i][j] == 10) {
          blue += 1;
        }
      }
  }
  //

  system("clear"); // コンソールのクリア
  printf("generation = %d, red = %d, blue = %d\r\n", gen, red, blue);
  printf("birth = %d, death = %d, war = %d\r\n", number[0], number[1], number[2]);
  if (red == 0 && blue == 0) {
    printf("\x1b[31mextinct!\r\n\x1b[0m");
  }
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
      } else if (cell[i][j] == 1) {
        printf("\x1b[31m#\x1b[0m"); // 赤 のプリント
      } else { // 10 のとき
        printf("\x1b[34m#\x1b[0m"); // 青 のプリント
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

void update_cells_4(const int height, const int width, int cell[height][width], int number[3]) {
  int cell_expanded[height + 2][width + 2];
  // 全てのセルを対等に扱うために盤面をexpand
  for (int i = 0; i < height + 2; i++) {
      for (int j = 0; j < width + 2; j++) {
        cell_expanded[i][j] = 0;
      }
  }
  // 現在の盤面をコピー
  for (int i = 1; i < height + 1; i++) {
      for (int j = 1; j < width + 1; j++) {
        cell_expanded[i][j] = cell[i - 1][j - 1];
      }
  }

  // 世代交代
  // まずは射程範囲に敵がいるかどうか
  // いたらその数的比率によって生死が分かれる
  // 倒す確率ではなく死ぬ確率！
  // いない場合は探索して移動

  int flag = 0;
  int count_red = 0;
  int count_blue = 0;
  for (int i = 1; i < height + 1; i++) {
    for (int j = 1; j < width + 1; j++) {
      if (cell_expanded[i][j] == 1) {
        count_red++;
      } else if (cell_expanded[i][j] == 10) {
        count_blue++;
      }
    }
  }
  if (count_red > 0 && count_blue > 0) {
    flag = 100; // 戦争フラグ
  } else if (count_red > 0) {
    flag = 1;
  } else {
    flag = 10;
  }

  // 戦争時のムーブ
  if (flag == 100) {

    // 乱数の準備
    srand((unsigned)time(NULL));

    for (int i = 1; i < height + 1; i++) {
      for (int j = 1; j < width + 1; j++) {

        if (cell_expanded[i][j] == 0) {
          continue; // 空き地なら次のマスへ
        }

        // 人が存在するとき
        int count = 0; // 敵の数を数える
        count += cell_expanded[i - 1][j - 1];
        count += cell_expanded[i - 1][j];
        count += cell_expanded[i - 1][j + 1];
        count += cell_expanded[i][j - 1];
        count += cell_expanded[i][j + 1];
        count += cell_expanded[i + 1][j - 1];
        count += cell_expanded[i + 1][j];
        count += cell_expanded[i + 1][j + 1];

        if (cell_expanded[i][j] == 1) { // 赤のとき
          count /= 10; // 敵（青）の数
        }
        else { // 青のとき
          count %= 10; // 敵（赤）の数
        }

        if (count > 0) { // 周囲に一人でも敵がいるとき
          double p = count / 8.0; // 殺される確率    // 型変換？？？？？？？
          double x = (double)rand() / RAND_MAX; // 0 ~ 1
          if (x < p) {
            cell[i - 1][j - 1] = 0; // 戦死 // 更新するのはcell！！！！！！！！！
            number[1]++;
          }
        }
        else { // 周囲に敵がいないとき

          // 最寄りの敵を探す
          int i_enemy = 200;
          int j_enemy = 200;
          for (int i2 = 1; i2 < height + 1; i2++) {
            for (int j2 = 1; j2 < width + 1; j2++) {
              if (i2 == i && j2 == j) { // 自分自身のマスは飛ばす
                continue;
              }
              else if (cell_expanded[i2][j2] != 0 && cell_expanded[i][j] != cell_expanded[i2][j2]) {// 注目地点に"敵"がいるとき
                if ((i - i2)*(i - i2) + (j - j2)*(j - j2) < (i - i_enemy)*(i - i_enemy) + (j - j_enemy)*(j - j_enemy)) {
                  i_enemy = i2;
                  j_enemy = j2;
                } // 少なくとも一人は敵がいるはず！
              }
            }
          }

          // 敵に最も近づける位置をサーチ
          int coor[8][2] = {{i - 1, j - 1}, {i - 1, j}, {i - 1, j + 1}, {i, j - 1}, {i, j + 1}, {i + 1, j - 1}, {i + 1, j}, {i + 1, j + 1}};
          int i_next = i; // とりあえずstayを仮定
          int j_next = j; // 同上
          for (int a = 0; a < 8; a++) {
            if ((coor[a][0] - i_enemy)*(coor[a][0] - i_enemy) + (coor[a][1] - j_enemy)*(coor[a][1] - j_enemy) < (i_next - i_enemy)*(i_next - i_enemy) + (j_next - j_enemy)*(j_next - j_enemy)) {
              // 下の条件を満たせばそこに動ける！
              if (cell[coor[a][0] - 1][coor[a][1] - 1] == 0 && (coor[a][0] > 0 && coor[a][0] < height + 1 && coor[a][1] > 0 && coor[a][1] < width + 1)) {
                i_next = coor[a][0];
                j_next = coor[a][1];
              }
            }
          }
          cell[i - 1][j - 1] = 0;
          cell[i_next - 1][j_next - 1] = cell_expanded[i][j];
        }

      }
    }
  }

  // 平穏時代のムーブ
  if (flag != 100) {
    // とりあえず移動
    // 移動するつもりだったところに誰かいたらstay
    // 移動が終わった後のループで空のマスに誕生
    // ratio が 0.3 を超えたら 上下片方の色を転換する！！

    // 乱数の準備
    srand((unsigned)time(NULL));
    double x = (double)rand() / RAND_MAX; // 0 ~ 1
    
    // 1.移動
    for (int i = 1; i < height + 1; i++) {
      for (int j = 1; j < width + 1; j++) {

        if (cell_expanded[i][j] == 0) {
          continue; // 空き地なら次のマスへ
        }

        int coor[9][2] = {{i - 1, j - 1}, {i - 1, j}, {i - 1, j + 1}, {i, j - 1}, {i, j}, {i, j + 1}, {i + 1, j - 1}, {i + 1, j}, {i + 1, j + 1}};
        int i_next = i;
        int j_next = j;
        x = (double)rand() / RAND_MAX; // 0 ~ 1
        x *= 9;
        int y = x; // 0 ~ 8 の整数
        if (cell[coor[y][0] - 1][coor[y][1] - 1] == 0 && (coor[y][0] > 0 && coor[y][0] < height + 1 && coor[y][1] > 0 && coor[y][1] < width + 1)) {
          i_next = coor[y][0];
          j_next = coor[y][1];
        }
        // 移動
        cell[i - 1][j - 1] = 0;
        cell[i_next - 1][j_next - 1] = cell_expanded[i][j];

      }
    }

    // 2.死亡
    for (int i = 0; i < height; i++) {
      for (int j = 0; j < width; j++) {
        x = (double)rand() / RAND_MAX; // 0 ~ 1
        if (x > 0.95) {
          if (cell[i][j] != 0) {
            number[1]++; // 死亡数カウント
          }
          cell[i][j] = 0;
        }
      }
    }

    // 3.誕生
    for (int i = 1; i < height + 1; i++) {
      for (int j = 1; j < width + 1; j++) {
        if (cell[i - 1][j - 1] != 0) {
          continue;
        }
        // 空のセルについて
        int coor[8][2] = {{i - 1, j - 1}, {i - 1, j}, {i - 1, j + 1}, {i, j - 1}, {i, j + 1}, {i + 1, j - 1}, {i + 1, j}, {i + 1, j + 1}};
        int parent = 0;
        for (int a = 0; a < 8; a++) {
          if (cell_expanded[coor[a][0]][coor[a][1]] != 0) { // 移動前の状態で、親が近くにいるか
            parent++;
          }
        }

        if (parent >= 2) {
          x = (double)rand() / RAND_MAX; // 0 ~ 1
          if (x > 0.9) {
            cell[i - 1][j - 1] = flag;
            number[0]++; // 誕生数カウント
          }
        }

      }
    }

    // ここで総数をカウントして、1割？を超えていたらセルの上半分を赤1、下半分を青10に！
    double ratio = 0;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
          if (cell[i][j] == 1 | cell[i][j] == 10) {
            ratio += 1;
          }
        }
    }
    ratio /= (height * width);

    // 戦争勃発ライン
    x = (double)rand() / RAND_MAX; // 0 ~ 1
    if (ratio > x) {
      for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
          if (cell[i][j] == 1 | cell[i][j] == 10) {
            x = (double)rand() / RAND_MAX; // 0 ~ 1
            if (x > 0.5) { // 赤
              cell[i][j] = 1;
            } else { // 青
              cell[i][j] = 10;
            }
          }
        }
      }
      number[2]++;
    }

    flag = 100;

  }

}