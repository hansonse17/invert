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
#include "./hash.c"
#include <pthread.h>

//delimiters used for tokenizing strings
#define DELIMITERS " @~`#&.,|[](^):;<>+!?_=/%$-\\\'\"\n\t\r\f\v"

//MAX_THREADS and BUCKET_REGIONS can be varied 
#define MAX_THREADS 25
#define BUCKET_REGIONS 25

//array that store locks
pthread_mutex_t lock_array[BUCKET_REGIONS];

//struct for arguments to use in pthread_create()
typedef struct thread_args {
  char* path;
} thread_args_t;

/* structs for file_node and list structures just like
   typical node and list struct */
typedef struct file_node {
  char * path;
  struct file_node* next;
} file_node_t;

typedef struct file_list {
  file_node_t * head;
  int count;
} file_list_t;


/* alreadyIndexed function is for checking whether line_to_file exists in the
   files in the bucket_dir
*/
int alreadyIndexed(char* line_to_file, char* bucket_dir, int bucket_region) {
  FILE *fptr;
  char buffer[256];
  if((fptr = fopen(bucket_dir, "r")) == NULL) {
    return 0; //false
  } else {
    while(fgets(buffer, 256, fptr)) {
      if(strcasecmp(line_to_file, buffer) == 0) {
        fclose(fptr);
        return 1; //true
      }
    }
    fclose(fptr);
    return 0; //false
  }
}

/* sendToBucket checks whether the word is already indexed, 
   and then opens the bucket file and writes the word
   together with its location to the bucket file
*/
void sendToBucket (char* word, char* word_location) {
      
  // printf("Your word is: %s : %s\n", word, word_location);
  char* bucket_name = hash(word);
  int bucket_region = findBucketRegion(bucket_name);

  //ignore if either or both of the first two characters are not alphanumerical
  if((!isalnum(bucket_name[0])) || (!isalnum(bucket_name[1]))) {
      
    return;
  }
  char* bucket_dir = findBucket(bucket_name);
  char* line_to_file = malloc(strlen(word) + strlen(word_location) + 3);
  strcpy(line_to_file, word);
  strcat(line_to_file, " ");
  strcat(line_to_file, word_location);
  strcat(line_to_file, "\n\0");
  
  pthread_mutex_lock(&lock_array[bucket_region]); //LOCK!!!
  if(!alreadyIndexed(line_to_file, bucket_dir, bucket_region)){
    int fd = open(bucket_dir, O_CREAT|O_APPEND|O_WRONLY, S_IRUSR|S_IWUSR);
    if (fd > -1) {
      write(fd, line_to_file, strlen(line_to_file));
    } else {
      printf("Error opening bucket file : %s\n", bucket_dir);
    }
    close(fd);
   
  }
  pthread_mutex_unlock(&lock_array[bucket_region]); //UNLOCK!!
  return;

}


/* indexFile function is the parallelized function that accesses 
   files and use the sendToBucket function to send the words 
   read from files in a given directory to the buckets
*/
void* indexFile (void* args) {

  thread_args_t* arguments = (thread_args_t*) args;
  char* file_location = arguments->path;
  char buffer[256];
  char* saveptr = buffer;
  char*  output;
  FILE *fptr;
  printf("Now indexing : %s\n", file_location);
  if((fptr = fopen(file_location, "r")) == NULL) {
    printf("Error opening file!\n");
    exit(1);
  }

  while(fgets(buffer, 256, fptr)) {
    output = strtok_r(buffer, DELIMITERS, &saveptr);
    while(output) {   
      sendToBucket(output, file_location);
      output = strtok_r (NULL, DELIMITERS, &saveptr);    
    }
  }
  
  fclose(fptr);
  
  return NULL;
}


/* indexDir function recursively goes through the files in a given directory
   and its subdirectories and apply indexFile on the files in that given 
   directory 
*/
void indexDir(char* path, file_list_t* files) {
  DIR *dp = opendir(path); 
  struct dirent *ep;


  if (dp != NULL) {
    while ((ep = readdir(dp))) {
      struct stat buf;
      char * name = ep->d_name;
      char * path_name = malloc(strlen(path)+strlen(name)+2);
      strcpy(path_name, path);
      strcat(path_name, "/");
      strcat(path_name, name);
      stat((const char*)path_name, &buf);
      //printf("%s %d\n", path_name, S_ISDIR(buf.st_mode));
      if ((strcmp(name, ".") == 0) || (strcmp(name, "..") == 0)) {
	//if current directory entry is current or parent
        //do nothing
      } else if(S_ISDIR(buf.st_mode) == 1) {
	//if current directory entry is another direcory 
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

/* min function returns the lesser of the  
   two parameters a and b
*/
int min(int a, int b) {
  if(a <= b) return a;
  return b;
}

int main (int argc, char** argv) {
  if(argc < 2) {
    printf("Please provide a directory location.\n");
    return 0;
  }
  //initializing file list
  file_list_t* files = malloc(sizeof(file_list_t));
  files->head = NULL;
  files->count = 0;
  //initializing locks
  for(int i=0; i<BUCKET_REGIONS; i++) {
    pthread_mutex_init(&lock_array[i], NULL);
  }
  
  char* path = argv[1]; 

  indexDir(path, files);
  printf("File count : %d\n", files->count);
  int num_files = files->count;
  int remaining_files = num_files;
  file_node_t* pnt = files->head;
  while(remaining_files > 0) {
    int thread_count = min(remaining_files, MAX_THREADS);
    printf("thread count : %d\n", thread_count); 
    pthread_t threads[thread_count];
    int i = 0;
    while((pnt != NULL) && (i < thread_count)) {
      thread_args_t* args = malloc(sizeof(thread_args_t));
      char* this_path = malloc(sizeof(char)*strlen(pnt->path)+1);
      strcpy(this_path, pnt->path);
      args->path = this_path;
      
      if(pthread_create(&threads[i++], NULL, indexFile, args)) {
        perror("pthread_create");
        exit(2);
        } 
      pnt = pnt->next;
    } 
    i = 0;
    pnt = files->head;
    while((pnt != NULL) && (i < thread_count)) {
      if(pthread_join(threads[i++], NULL)) {
        perror("pthread_join");
        exit(2);
      }
      pnt = pnt->next;//pnt shoudd end up where the thread creation left off
    } 
    remaining_files -= thread_count;
  }
  //destroy locks
  for(int i=0; i<BUCKET_REGIONS; i++) {
    pthread_mutex_destroy(&lock_array[i]);
  }
 
  return 0;
}

