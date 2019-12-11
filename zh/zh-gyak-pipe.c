#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

const int NUM_CHILDREN = 2;
const int MSG_LEN = 5;

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
        kill(getppid(), SIGUSR1);
        // from parent
        int a;
        read(pipe_p_to_c1[0], &a, sizeof(int));
        printf("Child1:%i\n", a);
        // from parent
        read(pipe_p_to_c1[0], &a, sizeof(int));
        printf("Child1:%i\n", a);
        // from child2
        read(pipe_c1_to_c2[0], &a, sizeof(int));
        printf("Child1:%i\n", a);
        exit(0);
      } else if (i == 1) {
        // child2
        kill(getppid(), SIGUSR2);
        // from parent
        int a;
        read(pipe_p_to_c2[0], &a, sizeof(int));
        printf("Child2:%i\n", a);
        // to brotha
        a = 8;
        write(pipe_c1_to_c2[1], &a, sizeof(int));
        // to parent
        a = 12;
        write(pipe_p_to_c2[1], &a, sizeof(int));
        kill(getppid(), SIGUSR2);
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
  int a = 5;
  // to child1
  write(pipe_p_to_c1[1], &a, sizeof(int));
  // to child2
  a = 36;
  write(pipe_p_to_c2[1], &a, sizeof(int));
  // to child1
  a = 58;
  write(pipe_p_to_c1[1], &a, sizeof(int));

  pause();
  // from child2
  read(pipe_p_to_c2[0], &a, sizeof(int));
  printf("Parent:%i\n", a);
  // wait for children to exit
  wait(NULL);
  wait(NULL);

  return 0;
}
