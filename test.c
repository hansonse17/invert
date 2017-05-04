#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <regex.h>

#define BUCKET_LOCATION "./buckets"

char* all_cap (char * string) {
  char tmp[2];// = "st";//(char )string;
  memcpy(tmp, string, sizeof(char)*3);
  char* ret = malloc(strlen(string) + 1);
  int i = 0; 
  while(tmp[i]) {
    tmp[i]=(char) toupper(tmp[i]);
    i++;
  }
  memcpy(ret, tmp, sizeof(char)*3);
  return ret;
} 


char* hash(char* word) {
  //take out non word characters from 'word'

  char tmp[256];
  strcpy(tmp, word);
  if(isalpha(tmp[0]) || isdigit(tmp[0])) {
    if(isalpha(tmp[1]) || isdigit(tmp[1])) {
      char* ret = malloc(sizeof(char) * 3);
      memcpy(ret, word, 2);
      strcat(ret, "\0");
      return all_cap(ret);
    }}
  return "xx"; 
}


void main() {
  printf("Hashed : %s\n", hash("aWllo"));
  printf("%d\n", isdigit('a'));
  printf("%d\n", isdigit('9'));
  printf("%d\n", (int) ('9'));
  printf("%d\n", isdigit('%'));
}
