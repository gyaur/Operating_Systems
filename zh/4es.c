#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <wait.h>

#define SIG_NOT_ACCEPTED SIGUSR1
#define SIG_ACCEPTED SIGUSR2

const int NUM_CHILDREN = 2;
const int MSG_LEN = 5;

struct dolg {
  char name[50];
  char title[50];
  int year;
  char tv_name[50];
};
int gsig = -1;

void handler(int sig) {
  if (sig == SIG_ACCEPTED) {
    gsig = sig;
  } else if (sig == SIG_NOT_ACCEPTED) {
    gsig = sig;
  }
}

int main(int argc, char const *argv[]) {
  char *kij_tv;
  int mem_id = shmget(IPC_PRIVATE, 500, IPC_CREAT | S_IRUSR | S_IWUSR);
  kij_tv = shmat(mem_id, NULL, 0);

  signal(SIGRTMIN, handler);
  signal(SIG_NOT_ACCEPTED, handler);
  signal(SIG_ACCEPTED, handler);

  srand(time(NULL));

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
        kill(getppid(), SIGRTMIN);
        // from parent
        struct dolg a;
        a.year = 2019;
        strcpy(a.name, "Nagy Lajos");
        strcpy(a.title, "Fradi sejtés");
        strcpy(a.tv_name, "Valaki");
        write(pipe_p_to_c1[1], &a, sizeof(struct dolg));
        pid_t pid = getpid();
        write(pipe_p_to_c1[1], &pid, sizeof(pid_t));
        pause();
        int r;
        if (gsig == SIG_ACCEPTED) {
          printf("Hallgato: Accepted\n");
        } else if (gsig == SIG_NOT_ACCEPTED) {
          printf("Hallgato: NOT Accepted\n");
          r = rand() % 2;
          if (r == 0) {
            printf("Akkor %s lesz a tv\n", kij_tv);
          } else {
            strcpy(kij_tv, "Kis Béla");
            printf("Legyen inkabb Kis Béla\n");
          }
          shmdt(kij_tv);
        } else {
          perror(":(");
          exit(EXIT_FAILURE);
        }
        gsig = -1;

        exit(0);
      } else if (i == 1) {
        // child2
        // témavez
        kill(getppid(), SIGRTMIN);
        // from parent
        struct dolg a;
        read(pipe_p_to_c2[0], &a, sizeof(struct dolg));
        printf("Got this:%s , %s, %s ,%i\n", a.title, a.name, a.tv_name,
               a.year);
        int r = rand() % 10;
        if (r < 6) {
          printf("TV: NOT Accepted\n");
          kill(getppid(), SIG_NOT_ACCEPTED);
        } else {
          printf("TV: Accepted\n");
          kill(getppid(), SIG_ACCEPTED);
        }

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
  pid_t child1;
  struct dolg a;
  read(pipe_p_to_c1[0], &a, sizeof(struct dolg));
  read(pipe_p_to_c1[0], &child1, sizeof(pid_t));
  write(pipe_p_to_c2[1], &a, sizeof(struct dolg));
  pause();
  if (gsig == SIG_ACCEPTED) {
    kill(child1, SIG_ACCEPTED);
  } else if (gsig == SIG_NOT_ACCEPTED) {
    strcpy(kij_tv, "Kis Peter");
    shmdt(kij_tv);
    kill(child1, SIG_NOT_ACCEPTED);
  } else {
    perror("asd");
    exit(EXIT_FAILURE);
  }
  gsig = -1;
  // wait for children to exit
  wait(NULL);
  wait(NULL);
  shmctl(mem_id, IPC_RMID, NULL);

  return 0;
}
