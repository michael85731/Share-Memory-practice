#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

int main(void){
  char input[50];
  int shm_fd;
  void *adr;

  //create share memory
  if((shm_fd = shm_open("/words", O_CREAT | O_RDWR, 0644)) < 0)
    perror("create share memory");
  if(ftruncate(shm_fd, 4096) < 0)
    perror("set share memory size");
  if((adr = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0)) < 0)
    perror("map share memory");

  bool exit = false;
  while(!exit){
    memset(input, '\0', sizeof(input));
    fgets(input, sizeof(input), stdin);

    sprintf(adr, "%s", input);
    adr += strlen(input);

    if(strlen(input) <= 2 && input[0] == 'e'){
      exit = true;
      break;
    }

    //如果user只輸入一個e，則退出程式
    if(exit)
      break;
  }

  //relese share memory
  shm_unlink("/words");

  return 0;
}
