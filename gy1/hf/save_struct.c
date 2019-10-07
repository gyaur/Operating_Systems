#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct saves {
  char name[40];
  int age;
};

int main() {
  struct saves test;
  FILE *f;

  f = fopen("file.p", "rb");
  if (f == NULL) {
    perror("File opening error\n");
    exit(1);
  }
  fread(&test, sizeof(test), 1, f);
  fclose(f);

  printf("%s, %i", test.name, test.age);

  return 0;
}