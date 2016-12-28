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
  int common_shm_fd;
  void *common_adr;
  int my_shm_fd;
  void *my_adr;

  //create its own share memory
  if((my_shm_fd = shm_open("/myWords", O_CREAT | O_RDWR, 0644)) < 0)
    perror("create share memory");
  if(ftruncate(my_shm_fd, 4096) < 0)
    perror("set share memory size");
  if((my_adr = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, my_shm_fd, 0)) < 0)
    perror("map share memory");

  //open share memory
  if((common_shm_fd = shm_open("/words", O_RDONLY, 0444)) < 0)
    perror("open share memory");
  if((common_adr = mmap(NULL, 4096, PROT_READ, MAP_SHARED, common_shm_fd, 0)) < 0)
    perror("map share memory");

  //read share memory and write into my own memory
  bool exit = false;
  void *start = my_adr;

  while(!exit){
    //刷新目前畫面
    printf("\033[?25l");
    for(int i = 0;i < 100;i++){
      printf("\033[K");
      printf("\033[1B");
    }
    printf("\33[0;0H");

    char origin[1000];
    char last_char;
    char last_pound;
    strcpy(origin, common_adr);
    last_char = origin[strlen(origin) - 2];
    char *common_iterator = strtok(origin, "\n");
    bool show_all_flag = false;
    char search_word[100];

    //讀取b程式的share memory，並偵測各式輸入
    while( common_iterator != NULL){
      if((common_iterator[0] == '@') && (last_char == '@') && (strlen(common_iterator) == 1)){
        //偵測是否有要印出目前share memory內容，有的話最後在全部印出
        show_all_flag = true;
      }else if(common_iterator[0] == 'e' && strlen(common_iterator) == 1){
        //偵測是否有開頭為e且長度只有一個，離開程式
        exit = true;
      }else if(!((strchr(common_iterator, '@') && strlen(common_iterator) == 1) || (common_iterator[0] == '#'))){
        //若不是只有@或開頭為#，就存入自己的share memory中
        char temp[80];
        strcpy(temp, common_iterator);
        sprintf(my_adr, "%s\n", temp);
        my_adr += strlen(temp) + 1;
      }else{
        //nothing
      }

      last_pound = common_iterator[0];
      memcpy(search_word, &common_iterator[1], strlen(common_iterator));

      common_iterator = strtok(NULL, "\n");
    }

    //自己的memory space pointer回到起始位置
    my_adr = start;

    //排序並印出所有字，以及其出現的次數
    char my_origin_data[4096];
    char *my_data[4096];
    int length = 0;
    strcpy(my_origin_data, my_adr);

    //把目前自己的share memory資料讀出，並用換行符號切割，然後把切割下的資料存進array
    char *my_iterator = strtok(my_origin_data, "\n");
    while(my_iterator != NULL){
      my_data[length] = my_iterator;

      my_iterator = strtok(NULL, "\n");
      length++;
    }

    //排序，泡泡排序法
    for(int i = 0; i < length;i++){
      for(int j = 0; j < length;j++){
        if(i == j){
          continue;
        }else{
          if(strcmp(my_data[i], my_data[j]) < 0){
            char *temp = my_data[i];
            my_data[i] = my_data[j];
            my_data[j] = temp;
          }
        }
      }
    }

    //統計出現次數
    char *keys[100];
    int times[100];
    int hash_iterator = 0; //給keys跟times用的iterator

    for(int i = 0;i < length;i++){
      int time = 0;
      for(int j = 0;j < length;j++){
        if(strcmp(my_data[i], my_data[j]) == 0){
          time++;
        }
      }

      if(i + 1 < length){
        if(strcmp(my_data[i], my_data[i + 1]) == 0){
          continue;
        }else{
          keys[hash_iterator] = my_data[i];
          times[hash_iterator] = time;
          hash_iterator++;
        }
      }else{
        keys[hash_iterator] = my_data[i];
        times[hash_iterator] = time;
        hash_iterator++;
      }
    }

    //搜尋，並印出結果
    if(last_pound == '#' && strlen(search_word) > 1){
      bool result = false;
      for(int i = 0;i < hash_iterator;i++){
        if(strcmp(search_word, my_data[i])){
          result = false;
        }else{
          result = true;
          break;
        }
      }

      printf("\33[K");

      if(result)
        printf("yes\n");
      else
        printf("no\n");
    }

    //印出排序結果及次數
    if(show_all_flag){
      for(int i = 0;i < hash_iterator;i++){
        printf("word: %-20s\ttimes: %d\n", keys[i], times[i]);
      }
    }

    //離開程式
    if(exit)
      break;
  }

  //釋放虛擬記憶體
  shm_unlink("/words");
  shm_unlink("/myWords");
  return 0;
}
