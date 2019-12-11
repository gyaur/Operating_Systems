#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

int child_count = 2;
int NUMBER_OF_PARTIES = 5;
void handler() {}
int main(int argc, char const *argv[]) {
  signal(SIGUSR1, handler);
  signal(SIGUSR2, handler);
  srand(time(NULL));

  int pipefd[2];
  int pipefd2[2];
  int pipefd3[2];

  int count = atoi(argv[1]);

  if (pipe(pipefd) == -1) {
    perror("Hiba a pipe nyitaskor!");
    exit(EXIT_FAILURE);
  }
  if (pipe(pipefd2) == -1) {
    perror("Hiba a pipe nyitaskor!");
    exit(EXIT_FAILURE);
  }
  if (pipe(pipefd3) == -1) {
    perror("Hiba a pipe nyitaskor!");
    exit(EXIT_FAILURE);
  }
  for (int i = 0; i < child_count; i++) {

    pid_t pid = fork();
    if (pid == 0) {
      // child
      if (i == 0) {
        // child1
        kill(getppid(), SIGUSR1);
        int r;
        close(pipefd[1]);
        printf("Ellenőrző: Fogadva:\n");
        int nums[count];
        for (int j = 0; j < count; j++) {
          read(pipefd[0], &r, sizeof(int));
          nums[j] = r;
          printf("%i\n", r);
        }
        close(pipefd[0]);
        int chance;
        int pos = 1;
        int neg = 0;
        for (int j = 0; j < count; j++) {
          write(pipefd2[1], &nums[j], sizeof(int));
          chance = rand() % 5;
          if (chance == 0)
            write(pipefd2[1], &neg, sizeof(int));
          else
            write(pipefd2[1], &pos, sizeof(int));
        }
        exit(0);

      } else {
        // child2
        int acc, resp;
        double bad_sum = 0;
        for (int j = 0; j < count; j++) {
          read(pipefd2[0], &acc, sizeof(int));
          read(pipefd2[0], &resp, sizeof(int));
          if (resp == 0) {
            printf("%i nem szavazhat\n", acc);
            ++bad_sum;
          } else
            printf("%i szavazhat\n", acc);
        }
        write(pipefd3[1], &bad_sum, sizeof(double));
        int vote;
        for (int j = 0; j < count - bad_sum; j++) {
          vote = rand() % NUMBER_OF_PARTIES;
          write(pipefd3[1], &vote, sizeof(vote));
        }

        exit(0);
      }

    } else if (pid > 0) {
      // parent
      if (i == 0) {
        pause();
      }

    } else {
      perror("fucked");
    }
  }

  close(pipefd[0]);
  int nums[count];
  for (size_t j = 0; j < count; j++) {
    nums[j] = rand();
    write(pipefd[1], &nums[j], sizeof(int));
  }
  close(pipefd[1]);
  double c;
  wait(NULL);
  wait(NULL);

  read(pipefd3[0], &c, sizeof(double));
  int number_of_votes = count - c;
  printf("%f százalék szavazhat\n", (1 - (c / count)) * 100);
  int votes[NUMBER_OF_PARTIES];
  for (int i = 0; i < NUMBER_OF_PARTIES; i++) {
    votes[i] = 0;
  }
  int vote;
  for (int i = 0; i < count - c; i++) {
    read(pipefd3[0], &vote, sizeof(int));
    votes[vote] += 1;
  }

  int max, maxind;
  max = votes[0];
  maxind = 0;
  for (int i = 1; i < NUMBER_OF_PARTIES; i++) {
    if (votes[i] > max) {
      max = votes[i];
      maxind = i;
    }
  }
  printf("A %i. párt nyert %i szavazattal\n", maxind, max);

  return 0;
}
