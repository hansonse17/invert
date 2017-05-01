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
//TO DO: FIGURE OUT REGULAR EXPRESSIONS (linux.die.net/man/3/regcomp)
//TO DO: don't save multiple entries for same word same file 
  //take out non word characters from 'word'
  regex_t regex;
  if(regcomp(&regex, "\W", 0)) {
    printf("Error, could not compile regex\n");
    exit(1);
  }
  
  
  char* ret = malloc(sizeof(char) * 3);
  memcpy(ret, word, 2);
  strcat(ret, "\0");
  return all_cap(ret);
}

char* findBucket(char* word) {
  char * ret = malloc(strlen(BUCKET_LOCATION) + 4);
  strcpy(ret, BUCKET_LOCATION);
  strcat(ret, "/");
  strcat(ret, hash(word));
  return ret;
}

void sendToBucket (char* word, char* word_location) {
  printf("Your word is: %s\n", word);
  char* bucket_dir = findBucket(word);

//  printf("%s\n", bucket_dir);
  
  char* line_to_file = malloc(strlen(word) + strlen(word_location) + 3);
  strcpy(line_to_file, word);
  strcat(line_to_file, " ");
  strcat(line_to_file, word_location);
  strcat(line_to_file, "\n\0");
  //LOCK!!!
  int fd = open(bucket_dir, O_CREAT|O_APPEND|O_WRONLY, S_IRUSR|S_IWUSR);
  if (fd > -1) {
    write(fd, line_to_file, strlen(line_to_file));
  } else {
    printf("Error opening file\n");
  }
  close(fd);
  //UNLOCK!!
  
  return;

}


void index_file (char* file_location) {
  char output[256];
//  char output2[256];
  FILE *fptr;

  if((fptr = fopen(file_location, "r")) == NULL) {
    printf("Error opening file!\n");
    exit(1);
  }
  
  while(fscanf(fptr, "%s", output) != EOF) {
//    printf("output : %s\n", output);
    sendToBucket(output, file_location);
  }
  fclose(fptr);

  return;

}


int main (void) {

  char* path = ("./sample");
  
 // printdir(path);

  //sendToBucket("it is cold", "./world.txt");

  index_file("./sample/forest.txt");
  
  
  return 0;
}
