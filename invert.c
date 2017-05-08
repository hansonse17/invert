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
#include <pthread.h>


#define DELIMITERS " @~`#&.,|[](^):;<>+!?_=/%$-\\\'\"\n\t"

typedef struct thread_args {
  char* path;
} thread_args_t;

typedef struct lock_node {
  pthread_mutex_t* lock;
  struct lock_node_t* next;
} lock_node_t;

typedef struct lock_list {
  lock_node_t* head;
} lock_list_t;

typedef struct file_node {
  char * path;
  struct file_node* next;
} file_node_t;

typedef struct file_list {
  file_node_t * head;
  int count;
} file_list_t;


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


  if(!alreadyIndexed(line_to_file, bucket_dir)){
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

void* kernel_test(void* args) {
  thread_args_t* arguments = (thread_args_t*) args; 
  //printf("You in a thread!\n");
  printf("Given path : %s\n", arguments->path);
  return NULL;
}

void* indexFile (void* args) {
  thread_args_t* arguments = (thread_args_t*) args;
  char* file_location = arguments->path;
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
  return NULL;
}

void indexDir(char* path, file_list_t* files) {
  DIR *dp = opendir(path); 
  struct dirent *ep;


  if (dp != NULL) {
    while ((ep = readdir(dp))) {
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
        indexDir(path_name, files);
      } else { // if current directory entry is a file
        file_node_t* new_node = malloc(sizeof(file_node_t));
        char* new_path = malloc(sizeof(char)*strlen(path_name)+1);
        strcpy(new_path, path_name);
        new_node->path = new_path;
        new_node->next = files->head;
        files->head = new_node;
        files->count = files->count + 1;
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
 
  file_list_t* files = malloc(sizeof(file_list_t));
  files->head = NULL;
  files->count = 0; 
  
  char* path = argv[1]; //("./sample");
/*
    lock_list_t* lockList = malloc(sizeof(lock_list_t)):
  lockList->head = NULL; */

  indexDir(path, files);
  printf("File count : %d\n", files->count);
  int num_files = files->count;
  pthread_t threads[num_files];
  file_node_t* pnt = files->head;
  int i = 0;
  while(pnt != NULL) {
    thread_args_t* args = malloc(sizeof(thread_args_t));
    char* this_path = malloc(sizeof(char)*strlen(pnt->path)+1);
    strcpy(this_path, pnt->path);
    args->path = this_path;
    if(pthread_create(&threads[i], NULL, indexFile, args)) {
      perror("pthread_create");
      exit(2);
    }
    pnt = pnt->next;
    i++;
  }
  i = 0;
  pnt = files->head;
  while(pnt != NULL) {
    if(pthread_join(threads[i], NULL)) {
      perror("pthread_join");
      exit(2);
    }
    pnt = pnt->next;
    i++;
  }

  

/*   
 * PRINT NAMES OF FILES
 *
  file_node_t* pnt = files->head;
  while(pnt != NULL) {
    printf("File : %s\n", pnt->path);
    pnt = pnt->next;
  } 
*
*/

/*  thread_node_t* pnt = threadList->head;
  while(pnt != NULL) {
    if(pthread_join(*(pnt->thread), NULL)){
      perror("error joining threads");
      exit(2);
    }
    pnt = pnt->next_thread;
  } */
  
  return 0;
}

