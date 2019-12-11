#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>

int main(int argc, char *argv[]) {

  int pid;
  key_t kulcs;
  char *s;
  int *sd;
  // a parancs nevevel es az 1 verzio szammal kulcs generalas
  // osztott memoria letrehozasa, irasra olvasasra, 500 bajt mérettel
  int oszt_mem_id = shmget(IPC_PRIVATE, 500, IPC_CREAT | S_IRUSR | S_IWUSR);
  int oszt_mem_id2 = shmget(IPC_PRIVATE, 500, IPC_CREAT | S_IRUSR | S_IWUSR);

  // csatlakozunk az osztott memoriahoz,
  // a 2. parameter akkor kell, ha sajat cimhez akarjuk illeszteni
  // a 3. paraméter lehet SHM_RDONLY, ekkor csak read van
  sd = shmat(oszt_mem_id, NULL, 0);
  s = shmat(oszt_mem_id2, NULL, 0);
  //
  pid = fork();
  if (pid > 0) {

    char buffer[] = "Hajra Újpest! \n";
    // beirunk a memoriaba
    strcpy(s, buffer);
    int a = 5;
    *sd = a;
    // elengedjuk az osztott memoriat
    shmdt(sd);
    shmdt(s);

    //	s[0]='B';  //ez segmentation fault hibat ad
    wait(NULL);
    // IPC_RMID- torolni akarjuk a memoriat, ekkor nem kell 3. parameter
    // IPC_STAT- osztott memoria adatlekerdezes a 3. parameterbe,
    //  ami shmid_ds struct tipusu mutato
    shmctl(oszt_mem_id, IPC_RMID, NULL);
    shmctl(oszt_mem_id2, IPC_RMID, NULL);

  } else if (pid == 0) {
    sleep(1);
    printf("A gyerek ezt olvasta az osztott memoriabol: %i,%s\n", *sd, s);
    // gyerek is elengedi
    shmdt(sd);
    shmdt(s);
  }

  return 0;
}
