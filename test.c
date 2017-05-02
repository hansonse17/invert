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




char* hash(char* word) {
//TO DO: FIGURE OUT REGULAR EXPRESSIONS (linux.die.net/man/3/regcomp)
//TO DO: don't save multiple entries for same word same file 
  //take out non word characters from 'word'
  regex_t regex;
  if(regcomp(&regex, "[^A-Za-z0-9_-]", REG_EXTENDED)) {
    printf("Error, could not compile regex\n");
    exit(1);
  }
  regmatch_t match[6];

  if(regexec(&regex, word, 5, match, 0)) {
    printf("No match!\n");
  }
  else {
    printf("Match!\n");
    printf("0 : %d\n", match[0].rm_so);
    printf("1 : %d\n", match[1].rm_so);
  }

  

  char tmp_2[3];
  char tmp_1[strlen(word)+1]
  char* ret = malloc(sizeof(char) * 3);
  memcpy(tmp_1, word, strlen(word));
  int i=0;
  while() {
    if(tmp_1[i] 

  }
  
  tmp[2]='\0';
  memcpy(ret, tmp, 3);
  return ret;
}

void main() {

  printf("no funny characters : %s\n", hash("5/1/2017"));
}
