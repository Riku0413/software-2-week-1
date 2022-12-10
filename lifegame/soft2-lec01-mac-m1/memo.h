
void init_cells(FILE* fp) {
  int h, w;
  char s[10];
  fscanf(fp, "%s", s);
  fscanf(fp, "%s", s); // 最初の2行?が余計な文字列
  while (fscanf(fp, "%d %d", &h, &w) != EOF) {
    printf("%d, %d\n", h, w);
  }
  //close the file.
  fclose(fp);
}