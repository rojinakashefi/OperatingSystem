#include "protocol.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

int main() {
  int fd = shm_open(NAME, O_CREAT | O_RDWR, 0600);
  if (fd < 0) {
    perror("shm_open()");
    return EXIT_FAILURE;
  }
  ftruncate(fd, SIZE);
  int *data =
      (int *)mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  data[0]=9;
  data[1]=8;
  data[2]=3;
  data[3]=1;
  data[4]=1;
  data[5]=1;
  data[6]=8;
  printf("sender mapped address: %p\n", data);
  munmap(data, SIZE);
  close(fd);

  return EXIT_SUCCESS;
}
