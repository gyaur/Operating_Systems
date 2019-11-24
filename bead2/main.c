#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

enum Locations { Bali, Mali, Cook_Island, Bahamas, Iceland };
enum Transport { Bus, Boat, Plane };
const char *locs[] = {"Bali", "Mali", "Cook Island", "Bahamas", "Iceland"};
const char *transp[] = {"Bus", "Boat", "Plane"};
const char *properties[] = {"Name", "Phone number", "Age", "Location",
                            "Transport"};
const int rescue_minimum[] = {1, 2, 3, 2, 2};
int rescue_arrived = 0;
int rescue_done = 0;

void handler(int sig) {
  if (sig == SIGUSR1)
    rescue_arrived = 1;
  else
    rescue_done = 1;
}

struct Tourist {
  char name[40];
  char phone[13];
  int age;
  enum Locations location;
  enum Transport transportation;
};

struct List {
  struct Tourist list[100];
  int size;
};

void readfile(struct List *to, char *fname) {
  FILE *f;

  f = fopen(fname, "rb");
  if (f == NULL) {
    printf("Creating file\n");
    FILE *g;
    g = fopen(fname, "wb");
    fclose(g);
  }
  fread(to, sizeof(struct List), 1, f);
  fclose(f);
}

void writefile(struct List to, char *fname) {
  FILE *f;
  f = fopen(fname, "wb");
  if (f == NULL) {
    perror("File opening error\n");
    exit(1);
  }
  fwrite(&to, sizeof(to), 1, f);
  fclose(f);
}
void print_tourist(struct Tourist t) {
  printf("%s , %s , %i, %s, %s\n", t.name, t.phone, t.age, locs[t.location],
         transp[t.transportation]);
}

void list(const char **arr) {
  for (int i = 0; arr[i] != 0; i++) {
    printf("%i. %s \n", i, arr[i]);
  }
}

void remove_tourist(struct List *tourists, unsigned int index) {
  if (tourists->size > 0 && index >= 0 && index < tourists->size) {
    tourists->size -= 1;
    tourists->list[index] = tourists->list[tourists->size];
  }
}
void add_tourist(struct List *tourists, struct Tourist t) {
  if (tourists->size < 100) {
    tourists->list[tourists->size] = t;
    tourists->size += 1;
  }
}

void list_tourists_at(struct List tourists, enum Locations loc) {
  int count = 0;
  for (int i = 0; i < tourists.size; i++) {
    if (tourists.list[i].location == loc) {
      printf("%i: ", i);
      print_tourist(tourists.list[i]);
      count++;
    }
  }
  if (count == 0) {
    printf("No tourists in %s!\n", locs[loc]);
  }
}

void list_tourists(struct List tourists) {
  if (tourists.size < 1) {
    printf("No tourists!\n");
  } else {
    for (int i = 0; i < tourists.size; i++) {
      printf("%i: ", i);
      print_tourist(tourists.list[i]);
    }
  }
}

int count_tourists_at(enum Locations loc, struct List t) {
  int count = 0;
  for (int i = 0; i < t.size; i++) {
    if (t.list[i].location == loc) {
      count++;
    }
  }
  return count;
}

int select_action() {
  printf("Select action:\n");
  printf("1: Add tourist\n");
  printf("2: Delete tourist\n");
  printf("3: Modify tourist\n");
  printf("4: List tourists\n");
  printf("5: List tourists at a specific location\n");
  printf("6: Start rescue\n");
  printf("7: Exit\n");
  printf("Index of selected action [1-6]: ");
  int ret;
  scanf("%d", &ret);
  return ret;
}

struct Tourist build_tourist() {
  char name[40];
  char phone[13];
  int age;
  enum Locations loc;
  enum Transport transport;

  printf("Name: ");
  scanf("%s", name);

  printf("\nPhone number: ");
  scanf("%s", phone);

  printf("\nAge: ");
  scanf("%i", &age);

  printf("\nPossible locations:\n");
  list(locs);
  printf("\nIndex of location:");
  scanf("%i", &loc);

  printf("\nPossible means of transport:\n");
  list(transp);
  printf("\nIndex of transport: ");
  scanf("%i", &transport);

  struct Tourist t;
  strcpy(t.name, name);
  strcpy(t.phone, phone);
  t.age = age;
  t.location = loc;
  t.transportation = transport;

  printf("\n");

  return t;
}

void modify_tourist(struct List *tourists, int index) {
  int prop;
  char name[50];
  char phone[13];
  int age;
  enum Locations loc;
  enum Transport transport;
  struct Tourist *t = &tourists->list[index];
  if (index < 0 && index > tourists->size) {
    printf("Invalid input\n");
  } else {
    list(properties);
    printf("Index of selected property: ");
    scanf("%i", &prop);
    switch (prop) {
    case 0:
      printf("New name: ");
      scanf("%s", name);
      strcpy(t->name, name);
      break;
    case 1:
      printf("New phone number: ");
      scanf("%s", phone);
      strcpy(t->phone, phone);
      break;
    case 2:
      printf("New age: ");
      scanf("%i", &age);
      t->age = age;
      break;
    case 3:
      list(locs);
      printf("Index of selected location: ");
      scanf("%i", &loc);
      t->location = loc;
      break;
    case 4:
      list(transp);
      printf("Index of selected transport: ");
      scanf("%i", &transport);
      t->transportation = transport;
      break;

    default:
      printf("Invalid input");
      break;
    }
  }
}

enum Locations check_rescue(struct List t) {
  for (int i = 0; i < 5; i++) {
    if (count_tourists_at(i, t) >= rescue_minimum[i])
      return i;
  }
  return -1;
}

void remove_tourists_at(struct List *t, enum Locations l) {
  for (int i = t->size - 1; i >= 0; i--) {
    if (t->list[i].location == l) {
      remove_tourist(t, i);
    }
  }
}

int main() {
  int ind;
  struct List tourists;
  readfile(&tourists, "file");
  // tourists.size = 0;
  // writefile(tourists, "file");
  printf("Tourist database\n");
  while (1) {
    int rescue = 0;
    switch (select_action()) {
    case 1:
      add_tourist(&tourists, build_tourist());
      break;
    case 2:
      if (tourists.size > 0) {
        list_tourists(tourists);
        printf("Index of selected tourist: ");
        scanf("%i", &ind);
        remove_tourist(&tourists, ind);
      } else
        printf("No tourists to delete\n");
      break;
    case 3:
      if (tourists.size > 0) {
        list_tourists(tourists);
        printf("Index of selected tourist: ");
        scanf("%i", &ind);
        modify_tourist(&tourists, ind);
      } else
        printf("No tourists to modify\n");
      break;
    case 4:
      list_tourists(tourists);
      break;
    case 5:
      list(locs);
      printf("Index of selected location: ");
      scanf("%i", &ind);
      list_tourists_at(tourists, ind);
      break;
    case 6:
      rescue = 1;
      break;
    case 7:
      writefile(tourists, "file");
      return 0;
      break;
    default:
      printf("Invalid input\n");
      break;
    }

    int l = check_rescue(tourists);
    if (l != -1 && rescue) {
      printf("Looking for people to rescue\n");
      printf("Starting rescue in %s:\n", locs[l]);
      signal(SIGUSR1, handler);
      signal(SIGUSR2, handler);

      int pipefd[2];

      if (pipe(pipefd) == -1) {
        perror("Hiba a pipe nyitaskor!");
        exit(EXIT_FAILURE);
      }
      // Start rescue
      pid_t pid = fork();
      if (pid < 0) {
        perror("fucked");
        exit(EXIT_FAILURE);

      } else if (pid == 0) {
        kill(getppid(), SIGUSR1);
        enum Locations loc;
        struct List tourists;
        read(pipefd[0], &loc, sizeof(loc));
        read(pipefd[0], &tourists, sizeof(tourists));
        // remove rescued tourists

        printf("Rescued:\n");
        list_tourists_at(tourists, loc);
        remove_tourists_at(&tourists, loc);
        write(pipefd[1], &tourists, sizeof(tourists));
        close(pipefd[1]);
        close(pipefd[0]);
        kill(getppid(), SIGUSR1);
        exit(0);

      } else {
        pause();
        write(pipefd[1], &l, sizeof(l));
        write(pipefd[1], &tourists, sizeof(tourists));
        pause();
        read(pipefd[0], &tourists, sizeof(tourists));
      }
    }
  }
  return 0;
}