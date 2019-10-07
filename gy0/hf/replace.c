#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *concat(char *f, char *s, char *t) {
  char *out = (char *)malloc(strlen(f) * strlen(s) * strlen(t) * sizeof(char));
  for (int i = 0; f[i] != 0; i++) {
    out[i] = f[i];
  }
  for (int i = 0; s[i] != 0; i++) {
    out[strlen(f) + i] = s[i];
  }
  for (int i = 0; t[i] != 0; i++) {
    out[strlen(f) + strlen(s) + i] = t[i];
  }
  return out;
}

char *get_string_range(char *array, int start, int end) {
  if (strlen(array) < end || strlen(array) < start) {
    return NULL;
  }
  char *out = (char *)malloc(end - start * sizeof(char));
  for (int i = start; i < end; i++) {
    out[i - start] = array[i];
  }
  return out;
}

char *replace(char *inp, char *find, char *change) {//add maloc
  int counter = 0;
  char *res;
  char *temp;
  if (strlen(inp) < strlen(find)) {
    return inp;
  }
  for (int i = 0; inp[i] != 0; i++) {
    if (inp[i] == find[0]) {
      res = get_string_range(inp, i, i + strlen(find));
      if (res != 0 && *res == *find) {
        temp = concat(get_string_range(inp, 0, i), change,
                      get_string_range(inp, i + strlen(find), strlen(inp)));
        inp = temp;
        counter++;
      }
    }
  }
  return inp;
}

int main(int argc, char const *argv[]) {
  if (argc == 4)
    printf("%s\n", replace(argv[1], argv[2], argv[3]));
  return 0;
}
