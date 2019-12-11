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

struct msg {
  long mtype;
  int msg;
  int something;
  char pass[5];
};

const int NUM_CHILDREN = 2;
const int MSG_LEN = 5;

void handler(int sig) {}

struct msg rec(int messagequeue, int id) {
  struct msg uz;
  int status;

  status = msgrcv(messagequeue, &uz, sizeof(struct msg), id, 0);

  if (status < 0)
    perror("msgrec");
  return uz;
}

int send(int messagequeue, int id, int msg, int smth, char *pass) {
  struct msg uz = {id, msg, smth};
  strncpy(uz.pass, pass, MSG_LEN);
  int status;

  status = msgsnd(messagequeue, &uz, sizeof(struct msg), 0);
  if (status < 0)
    perror("msgsnd");
  return 0;
}

int main(int argc, char const *argv[]) {
  signal(SIGUSR1, handler);
  signal(SIGUSR2, handler);

  int messagequeue;

  messagequeue = msgget(IPC_PRIVATE, 0600 | IPC_CREAT);

  if (messagequeue < 0) {
    perror("msgget");
    exit(1);
  }

  for (int i = 0; i < NUM_CHILDREN; ++i) {
    pid_t pid = fork();
    if (pid == 0) {
      // children
      if (i == 0) {
        // child1
        kill(getppid(), SIGUSR1);
        // from parent
        struct msg a = rec(messagequeue, 1);
        printf("Child1:%i ,%i, %s\n", a.something, a.msg, a.pass);
        // from parent
        a = rec(messagequeue, 1);
        printf("Child1:%i ,%i, %s\n", a.something, a.msg, a.pass);
        // from child2
        a = rec(messagequeue, 1);
        printf("Child1:%i ,%i, %s\n", a.something, a.msg, a.pass);
        exit(0);
      } else if (i == 1) {
        // child2
        kill(getppid(), SIGUSR2);
        // from parent
        struct msg a = rec(messagequeue, 2);
        printf("Child2:%i ,%i, %s\n", a.something, a.msg, a.pass);
        // to brotha
        send(messagequeue, 1, 123, 581, "bro");
        // to parent
        send(messagequeue, 3, 213, 158, "gy");
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
  // to child1
  send(messagequeue, 1, 1, 2, "----");
  // to child2
  send(messagequeue, 2, 21, 58, "sad");
  // to child1
  send(messagequeue, 1, 1, 2, "-**-");
  pause();
  // from child2
  struct msg a = rec(messagequeue, 3);
  printf("Parent:%i ,%i, %s\n", a.something, a.msg, a.pass);
  // wait for children to exit
  wait(NULL);
  wait(NULL);

  return 0;
}
