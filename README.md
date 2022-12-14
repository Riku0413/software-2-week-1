# software-2-week-1

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

初めに該当ファイル内で以下のコードを実行してコンパイル。
```
gcc -o mylife4 -Wall mylife4.c -L. -lgol  
```

続いて、以下のコードにより実行する。
```
./mylife4
```

### コードの解説

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