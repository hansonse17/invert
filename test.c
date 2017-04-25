#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

void printdir(char* path) {
  DIR *dp = opendir(path); 
  struct dirent *ep;

  if (dp != NULL) {
    while (ep = readdir(dp)) {
      struct stat buf;
      char * name = ep->d_name;
      char * path_name = path;
      strncat(path_name, name, sizeof(name));
      stat((const char*)path_name, &buf);
      printf("%s %d\n", path_name, S_ISDIR(buf.st_mode));
//      puts(ep->d_name);
    }
  (void) closedir (dp);
  }
  else { 
    perror ("didn't work\n");
  }
}

int main (void) {

  char* path = ("./");
  
  printdir(path);


 // struct stat buf;

 // stat(path, &buf);

 // printf("%d\n",S_ISDIR(buf.st_mode));//0 is false
  
  
  return 0;
}
