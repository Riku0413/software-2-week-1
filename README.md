# software-2-week-1

## このリポジトリについて

- 「工学部電子情報工学科 2022年度2Aセメスター講義 ソフトウェアⅡ」で作成したプログラム
- C言語によるCLIライフゲームを実装

## 課題1

argc == 1 のときにランダムにセルが配置されるように以下のコードを追加。
```
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
```

さらに、gol2.hのprint_cell_2関数内に以下のコードを追加。
これによって各世代において、生きたセルの割合がratioに格納される。
```
double ratio = 0;
for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      if (cell[i][j] == 1) {
        ratio += 1;
      }
    }
}
ratio /= (height * width);
```
     
ここで、さらに以下のコードを追加したことにより、世代番号のあとに生きたセル比率が表示される。
```
printf("generation = %d, ratio = %lf\r\n", gen, ratio);
```
## 課題2

mylife2.c の main関数内の世代を進めるループ内に以下のコードを追加。
まず、char型の配列を作り、そこに出力するファイル名を格納している。
続くforループでは、現在の世代番号に応じてファイル名を調整している。
最後にポインタを使ってファイルを開き、生きたセルが存在する座標を全て書き込んでいる。
```
if (gen % 100 == 0 && gen < 10000) {
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
```

## 課題3

mylife3.c が参照する gol3.h 内の init_cell_3関数の中身を以下のように調整。
```
void init_cells_3(const int height, const int width, int cell[height][width], FILE* fp) {
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
                if (data[i] == 'b') {
                  cell[line][raw] = 0;
                }
                if (data[i] == 'o') {
                  cell[line][raw] = 1;
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
```

fp != NULL のときの挙動について説明する。
まず、以下のコードによりのちに使うインスタンス生成とファイルの1行目の読み込みを行なっている・
```
int x, y;
char s[3000];
fscanf(fp, "%[^\n]", s); // 最初の\nまでをスキャン
```

続いて、以下のif文により、参照したファイルがLife1.06形式であるかどうかを判定している。仮にLife1.06形式の場合、以下のif文内のコードによって簡単にセル配置を初期化できる。
```
// #Life 1.06 の場合
if (s[1] == 'L' && s[2] == 'i') { // 十分条件

  while (fscanf(fp, "%d %d", &x, &y) != EOF) {
    cell[y][x] = 1;
  }
}
```

次に、参照ファイルがLife1.06形式ではなかった場合、つまりRLEフォーマットであった場合、else構文内でまず、以下のプログラムを動かす。これによって、RLEフォーマット内の最後のヘッダー文まで読み込みが完了したところでストップする。
```
// 説明文が終わるまでスキャン
if (s[0] != 'x') {
  while(1) {
    fscanf(fp, "%[^\n]", s);
    if (s[0] == 'x') {
      break;
    }
  }
}
```

続いて、以下のコードにより、ファイル内の本文、つまり符号化されたセル配置の情報を最後まで全て読み込む。さらにその後のプログラムで使用するインスタンスを用意しておく。ここで、data[100]はセル配置1行分の情報を格納する配列、p_sはs[3000]配列の要素を取り出すインデックス、p_dataはdata[100]配列の要素を取り出すインデックス、lineはセル配置を初期化するときに、何行目に焦点を当てているかを示す変数、rawは同様に何列目かを示す変数である。
```
// 本文の読み込み

// 一旦全部読み込み
fscanf(fp, "%[^!]", s);

char data[100];
int p_s = 0;
int p_data = 0;
int line = 0; // 何行目のセルかを指定
int raw = 0; // 何列目のセルかを指定
```

最後に、実際にセル配置を初期化するwhileループについて説明する。
まず、以下のコードにより、読み込んだ文字列内に改行かスペースがあった場合は読み飛ばすことにする。
```
if (s[p_s] == '\n' | s[p_s] == ' ') {
  p_s++;
  continue;
}
```

続いて、以下のコードにより、注目している行のセル配置情報をdata[100]配列に写す。
```
data[p_data] = s[p_s];
```

次に、以下のif文によって、ある行の情報が末尾に到達したときを判定する。ここでbuff[3]にはbかoのセットが何個連続しているかの情報を"文字列の形式で"逐一格納しておく。それをatoi()関数で整数に変換し、その個数だけ、続くforループ内でcell配置の情報を更新している。最後のif-else構文はある1行の初期配置更新が終わった場合に、次の行の更新作業に移ることと、最後の行の更新が終わったときにwhile文の無限ループを抜け出すことを示している。
```
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
        if (data[i] == 'b') {
          cell[line][raw] = 0;
        }
        if (data[i] == 'o') {
          cell[line][raw] = 1;
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
```
追記：高確率で原因不明の segmentation fault になってしまいますが、20回に1回ほど上手くいきます。

## 発展課題

### - 追加した機能 -

・セル配置の初期化時にランダムで赤陣営と青陣営に分かれる

・各セルが最寄りの敵陣営セルの方向に移動する

・赤と青のセルが接触すると、その勢力に応じた確率でセルが死滅

・片方の陣営が絶滅すると、戦争は終結し、繁殖と寿命による死亡が確率的に起こる

・繁殖により、一色のセルが増えすぎると確率的に戦争が勃発し赤陣営と青陣営に分かれる

・上述の現象が繰り返される

・生誕、死亡、戦争の総数をカウントして表示

・セルが絶滅したら"extinct!"と表示

### 実行方法

初めに以下のコードにより、ファイルのある階層まで移動。
```
cd ./lifegame/soft2-lec01-mac-m1
```

続いて以下のコードを実行してコンパイル。
```
gcc -o mylife4 -Wall mylife4.c -L. -lgol  
```

最後に、以下のコードにより実行する。
```
./mylife4
```

### コードの解説

まず、以下のコードにあるような乱数により、各マスについて5％の確率で赤のセル（整数の1に対応）、5%の確率で青のセル（整数の10に対応）が配置される。外部ファイルを読み込む場合についても、赤と青のセルは等確率で生じるようになっている。
```
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
```

続いて、以下のように、print_cell、update_cell関数の引数に、生誕、死亡、戦争の総数をカウントする配列number[3]を追加した。
```
void print_cells_4(FILE *fp, int gen, const int height, const int width, int cell[height][width], int number[3])

void update_cells_4(const int height, const int width, int cell[height][width], int number[3])
```

次に、print_cell関数によって盤面表示の前に出力する内容を、以下のように変更した。これによって、赤陣営、青陣営の生存数、累積の生誕、死亡、戦争数が表示される。さらにセルが絶滅した際には、赤い文字で"extinct!"が表示されることになる。
```
printf("generation = %d, red = %d, blue = %d\r\n", gen, red, blue);
printf("birth = %d, death = %d, war = %d\r\n", number[0], number[1], number[2]);
if (red == 0 && blue == 0) {
  printf("\x1b[31mextinct!\r\n\x1b[0m");
}
```

続いて、肝となるのupdata_cell関数について、まず以下のように、現在の赤陣営、青陣営の総数をカウントして、両方とも1つ以上存在していたら、flag = 100 とし、戦争状態になるようにした。片方のセルのみが存在する場合は、そのセルに相当する整数がflagに格納される。
```
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
```

ここで、戦争状態の挙動を示したコードは以下である。長いコードになっているが、一番外側の二重ループに関しては、盤面の各マスを一つ一つ順番に調べることを意味している。ここで、注目しているマスにセルが存在しない場合は、continueによりにより次のマスに移る。逆にセルが存在する場合は、さらに二つのケースに分岐する。まず、自分の周囲の8マスに一個体でも敵陣営がいる場合は、if文を通り、その数に比例した確率で自分が死滅する。敵が周囲にいない場合は、else文を通り、自分の現在位置から最も近い位置にいる敵陣営セルに近づく方向に移動する。
```
// 戦争時の挙動
if (flag == 100) {

  // 乱数の準備
  srand((unsigned)time(NULL));

  for (int i = 1; i < height + 1; i++) {
    for (int j = 1; j < width + 1; j++) {

      if (cell_expanded[i][j] == 0) {
        continue; // 空き地なら次のマスへ
      }

      // セルが存在するとき
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
        double p = count / 8.0; // 殺される確率
        double x = (double)rand() / RAND_MAX; // 0 ~ 1
        if (x < p) {
          cell[i - 1][j - 1] = 0; // 戦死
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
```

最後にセルが一色のみ存在する平和な時代のコードは以下である。このコードが動くのはflagが1か10のときに該当するが、主に3つのステップがある。まず、一つ目は「移動」であり、このステップでは、自分の周囲のマスへ移動するか、現在位置にとどまるかが確率的に起こる。このとき、すでに別のセルが存在するマスには移動しないように制御されている。続いて、二つ目は「死亡」であり、「移動」が終わった後、各セルが5%の確率で寿命で死滅することになる。最後に、三つ目は「誕生」であり、このステップでは「現在はセルが存在しないが、周囲にセルが二つ以上存在する」という条件を満たすマスにおいて、10%の確率で新たなセルが誕生する。以上の３ステップが終了した後、盤面全体の生きたセル割合が計算され、それが、確率的に定まる閾値xを超えたていた場合、約半分のセルの色が反転すると同時に、flag = 100 となり、戦争状態に入る。
```
// 平穏時代の挙動
if (flag != 100) {
  // とりあえず移動
  // 移動するつもりだったところに誰かいたらstay
  // 移動が終わった後のループで空のマスに誕生

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
```
