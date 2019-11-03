#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h> //waitpid
#include <unistd.h>

int main() {
  int pid, status;
  for (int i = 0; i < 5; i++) {
    pid = fork();
    if (pid < 0) // Error
      perror("Fucked\n");
    else if (pid == 0) { // Child
      printf("Hi, I'm one of the child processes: {%i}\n", getpid());
      exit(0);
    } else { // Parent
      wait(&status);
      printf("%i\n", status);
    }
  }

  return 0;
}