#define BUCKET_LOCATION "./buckets"
#define BUCKET_REGIONS 10

char* all_cap (char * string) {
  char tmp[3];// = "st";//(char )string;
  memcpy(tmp, string, sizeof(char)*3);
  char* ret = malloc(strlen(string) + 2);
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


  regex_t regex;
  regmatch_t match[2];
  if(regcomp(&regex, "[^A-Za-z0-9_-]", 0)) {
    printf("Error, could not compile regex\n");
    exit(1);
  }
  
  if(regexec(&regex, word, 1, match, 0)){
    char* ret = malloc(sizeof(char) * 3);
    strcpy(ret, word);
    //memcpy(ret, word, 2);
    //strcat(ret, "\0");
    if(strlen(word)<2) {
      ret[1] = 'x';
    }
    ret[2] = '\0';
    // printf("HASH %s : %s\n", word, ret);
    return all_cap(ret);}
  else {
    return "xx"; //x files: unexplainable phenomena 
  }
} 


char* findBucket(char* bucket_name) {
  char * ret = malloc(strlen(BUCKET_LOCATION) + 4);
  strcpy(ret, BUCKET_LOCATION);
  strcat(ret, "/");
  strcat(ret, bucket_name);
  return ret;
}


int findBucketRegion(char* bucket) {
  char indexable[3];
  strcpy(indexable, bucket);
  int first_char = indexable[0];
  if(isdigit(first_char)) { //48 <= first_char && first_char <= 57) {
    return 0;
  } 
  else if(isalpha(first_char)) { //65 <= indexable[0] && indexable[0] <= 70) {
    int region = (first_char%(BUCKET_REGIONS-2))+1;
    //  printf("Region %d\n", region);
    return region;
  }
  else {
    return BUCKET_REGIONS-1;
  }

}
