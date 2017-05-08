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
#include <./hash.c>
//#include <./invert.c>


void search(char* term) {
  char* bucket_location = findBucket(hash(term));
  FILE *fptr;
  char buffer[256];
  //Lock
  if((fptr = fopen(bucket_location, "r")) == NULL) {
    printf("Term not found!\n");
    exit(1);
  }
  int found = 0; //0 is not found
  while(fgets(buffer, 256, fptr)) {
    if(strncasecmp(buffer, term, strlen(term)) == 0) {
        printf("%s", buffer);
        found = 1;
    }
  }
  fclose(fptr);
  //Unlock
     
  if(found==0){
    printf("Term not found!\n");
  }
  
  return;
}
  
int main (int argc, char** argv) {
  if(argc < 2) {
    printf("Please provide a directory location.\n");
    return 0;
  }
  
  char* searchTerm = argv[1]; 
  

  search(searchTerm);
  
  return 0;
}
  
