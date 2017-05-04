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
#define DELIMITERS " @~`#&.,|[](^):;<>+!?_=/%$-\\\'\"\n\t"



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

/*
char* hash(char* word) {
  //take out non word characters from 'word'

  char tmp[256];
  strcpy(tmp, word);
  if(iscntrl(tmp[0]) || iscntrl(tmp[1])) {
    return "xx";
  }
  if(isalpha(tmp[0]) || isdigit(tmp[0])) {
    if(isalpha(tmp[1]) || isdigit(tmp[1])) {
      char* ret = malloc(sizeof(char) * 2);
      memcpy(ret, word, 2);
      strcat(ret, "\0");
      return all_cap(ret);
    }}
  return "xx"; 
} */

char* hash(char* word) {
  //take out non word characters from 'word'


  regex_t regex;
  regmatch_t match[2];
  if(regcomp(&regex, "[^A-Za-z0-9_-]", 0)) {
    printf("Error, could not compile regex\n");
    exit(1);
  }
  
  if(regexec(&regex, word, 1, match, 0)){
    char* ret = malloc(sizeof(char) * 3);
    memcpy(ret, word, 2);
    strcat(ret, "\0");
    return all_cap(ret);}
  else {
    return "xx"; //x files: unexplainable phenomena 
  }
} 

char* findBucket(char* word) {
  char * ret = malloc(strlen(BUCKET_LOCATION) + 4);
  strcpy(ret, BUCKET_LOCATION);
  strcat(ret, "/");
  strcat(ret, hash(word));
  return ret;
}

int alreadyIndexed(char* line_to_file, char* bucket_dir) {
  FILE *fptr;
  char buffer[256];
  //LOCK!
  if((fptr = fopen(bucket_dir, "r")) == NULL) {
    //UNLOCK
    return 0; //false
  } else {
    while(fgets(buffer, 256, fptr)) {
      if(strcasecmp(line_to_file, buffer) == 0) {
        //UNLOCK
        return 1; //true
      }
    }
    //UNLOCK
    return 0; //false
  }
}

void sendToBucket (char* word, char* word_location) {
 // printf("Your word is: %s\n", word);
  char* bucket_dir = findBucket(word);
//  printf("%s\n", bucket_dir);
  
  char* line_to_file = malloc(strlen(word) + strlen(word_location) + 3);
  strcpy(line_to_file, word);
  strcat(line_to_file, " ");
  strcat(line_to_file, word_location);
  strcat(line_to_file, "\n\0");


  if(!alreadyIndexed(line_to_file, bucket_dir)) {
    //LOCK!!!
    int fd = open(bucket_dir, O_CREAT|O_APPEND|O_WRONLY, S_IRUSR|S_IWUSR);
    if (fd > -1) {
      write(fd, line_to_file, strlen(line_to_file));
    } else {
      printf("Error opening bucket file : %s\n", bucket_dir);
    }
    close(fd);
    //UNLOCK!!
  }
  return;

}


void indexFile (char* file_location) {
  char buffer[256];
  char*  output;
  FILE *fptr;
  printf("Now indexing : %s\n", file_location);
  if((fptr = fopen(file_location, "r")) == NULL) {
    printf("Error opening file!\n");
    exit(1);
  }

  while(fgets(buffer, 256, fptr)) {
    output = strtok(buffer, DELIMITERS);
    while(output) {
      sendToBucket(output, file_location);
      output = strtok (NULL, DELIMITERS);
    }
  }
  fclose(fptr);
  return;
}

void indexDir(char* path) {
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
      //printf("%s %d\n", path_name, S_ISDIR(buf.st_mode));
      if ((strcmp(name, ".") == 0) || (strcmp(name, "..") == 0)) { //if current directory entry is current or parent
        //do nothing
      } else if(S_ISDIR(buf.st_mode) == 1) { //if current directory entry is another direcory 
        indexDir(path_name);
      } else { // if current directory entry is a file
        indexFile(path_name);
      }
    }
  (void) closedir (dp);
  }
  else { 
    perror ("Error opening directory\n");
  }
}
int main (int argc, char** argv) {
  if(argc < 2) {
    printf("Please provide a directory location.\n");
    return 0;
  }
  
  char* path = argv[1]; //("./sample");
  

  indexDir(path);
  
  return 0;
}
