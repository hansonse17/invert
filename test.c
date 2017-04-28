#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

void printdir(char* path) {
  DIR *dp = opendir(path); 
  struct dirent *ep;

  if (dp != NULL) {
    while (ep = readdir(dp)) {
      struct stat buf;
      char * name = ep->d_name;
      char * path_name = malloc(strlen(path)+strlen(name)+1);
      strcpy(path_name, path);
      strcat(path_name, "/");
      strcat(path_name, name);
      stat((const char*)path_name, &buf);
      printf("%s %d\n", path_name, S_ISDIR(buf.st_mode));
      if((S_ISDIR(buf.st_mode) == 1) && 
         (strcmp(name, ".")) && 
         (strcmp(name, ".."))) { //if current directory entry is another direcory
        printdir(path_name);
      }
    }
  (void) closedir (dp);
  }
  else { 
    perror ("didn't work\n");
  }
}

void all_cap (char * string) {
  char tmp[3] = "st";//(char )string;
  char* ret = malloc(strlen(string) + 1);
  int i = 0;
  while(tmp[i]) {
      putchar( toupper(tmp[i]));
    //  strcat(ret, (char)toupper(tmp[i]));
      i++;
  }
  printf("%s\n", ret);

  return;
} 

void sendToBucket (char* word, char* path) {
  char * bucket_dir = malloc(strlen("./buckets") + strlen(path) + 1);
  strcpy(bucket_dir, "./buckets");
  
  char * hash = malloc(sizeof(char) * 3);
  memcpy(hash, word, 2);
  strcat(hash, "\0");
  printf("Hash : %s\n", hash);
  
  all_cap(hash);
  
  return;

}


int main (void) {

  char* path = ("./sample");
  
 // printdir(path);

  sendToBucket("hello", "./world.txt");


  
  
  return 0;
}
