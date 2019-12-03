#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h> // for pipe()
                    //
                    // unnamed pipe example
                    //
int main(int argc, char *argv[]) {
  int pipefd[2]; // unnamed pipe file descriptor array
  pid_t pid;
  int f;
  int g;
  int res; // char array for reading from pipe

  if (pipe(pipefd) == -1) {
    perror("Hiba a pipe nyitaskor!");
    exit(EXIT_FAILURE);
  }
  pid = fork(); // creating parent-child processes
  if (pid == -1) {
    perror("Fork hiba");
    exit(EXIT_FAILURE);
  }

  if (pid == 0) { // child process
    sleep(3);     // sleeping a few seconds, not necessary
    // close(pipefd[1]); // Usually we close the unused write end
    printf("Gyerek elkezdi olvasni a csobol az adatokat!\n");
    read(pipefd[0], &f, sizeof(f));
    read(pipefd[0], &g, sizeof(g)); // reading max 100 chars
                                    // reading max 100 chars
    printf("Gyerek olvasta uzenet: %i + %i = %i", f, g, f + g);
    printf("\n");
    int res = f + g;
    write(pipefd[1], &res, sizeof(int));
    close(pipefd[0]);
    close(pipefd[1]); // finally we close the used read end
  } else {            // szulo process
    printf("Szulo indul!\n");
    int x = 15;
    int y = 2;
    int res1; // Usually we close unused read end
    write(pipefd[1], &x, sizeof(int));
    write(pipefd[1], &y, sizeof(int));
    printf("Szulo beirta az adatokat a csobe!\n");
    fflush(NULL); // flushes all write buffers (not necessary)
    wait(NULL);   // waiting for child process (not necessary)
                  // try it without wait()

    read(pipefd[0], &res1, sizeof(int));
    printf("\nres: %i\n", res1);

    close(pipefd[0]);
    close(pipefd[1]); // Closing write descriptor

    printf("Szulo befejezte!\n");
  }
  exit(EXIT_SUCCESS); // force exit, not necessary
}
