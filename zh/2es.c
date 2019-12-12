#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

const int NUM_CHILDREN = 2;
const int MSG_LEN = 5;

struct dolg {
  char name[50];
  char title[50];
  int year;
  char tv_name[50];
};

void handler(int sig) {}

int main(int argc, char const *argv[]) {
  signal(SIGUSR1, handler);
  signal(SIGUSR2, handler);

  int pipe_p_to_c1[2];
  int pipe_p_to_c2[2];
  int pipe_c1_to_c2[2];

  if (pipe(pipe_p_to_c1) == -1) {
    perror("Hiba a pipe nyitaskor!");
    exit(EXIT_FAILURE);
  }
  if (pipe(pipe_p_to_c2) == -1) {
    perror("Hiba a pipe nyitaskor!");
    exit(EXIT_FAILURE);
  }
  if (pipe(pipe_c1_to_c2) == -1) {
    perror("Hiba a pipe nyitaskor!");
    exit(EXIT_FAILURE);
  }

  for (int i = 0; i < NUM_CHILDREN; ++i) {
    pid_t pid = fork();
    if (pid == 0) {
      // children
      if (i == 0) {
        // child1
        // hallgató
        kill(getppid(), SIGUSR1);
        // from parent
        struct dolg a;
        a.year = 2019;
        strcpy(a.name, "Nagy Lajos");
        strcpy(a.title, "Fradi sejtés");
        strcpy(a.tv_name, "Valaki");
        write(pipe_p_to_c1[1], &a, sizeof(struct dolg));

        exit(0);
      } else if (i == 1) {
        // child2
        // témavez
        kill(getppid(), SIGUSR2);
        // from parent
        struct dolg a;
        read(pipe_p_to_c2[0], &a, sizeof(struct dolg));
        printf("Got this:%s , %s, %s ,%i\n", a.title, a.name, a.tv_name,
               a.year);

        exit(0);
      }
    } else if (pid > 0) {
      // parent
      /*
      if (i==0){
        pause();
      }
      */ // így csak az első gyereknél lesz pause
      pause();

    } else {
      perror("fucked");
    }
  }
  // real parent code
  // NEPTUN
  struct dolg a;
  read(pipe_p_to_c1[0], &a, sizeof(struct dolg));
  write(pipe_p_to_c2[1], &a, sizeof(struct dolg));

  // wait for children to exit
  wait(NULL);
  wait(NULL);

  return 0;
}
