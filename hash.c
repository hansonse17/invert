#define BUCKET_LOCATION "./buckets"
#define BUCKET_REGIONS 25

/* all_cap function takes in a word in the form of a string,
   take only the first two characters, capitalize them, 
   and return them as the output 
*/
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

/* hash function is to get the bucket file name for the input word
   using its first two characters 
*/

char* hash(char* word) {
  //take out non alphanumerical characters from 'word'
  regex_t regex;
  regmatch_t match[2];
  //ignore those that are not alphernumerical or _ or -
  if(regcomp(&regex, "[^A-Za-z0-9_-]", 0)) {
    printf("Error, could not compile regex\n");
    exit(1);
  }
  
  if(regexec(&regex, word, 1, match, 0)){
    char* ret = malloc(sizeof(char) * 3);
    strcpy(ret, word);
    //if the bucket filename is only one character long, append x
    if(strlen(word)<2) {
      ret[1] = 'x';
    }
    //add null character at the end
    ret[2] = '\0';
    return all_cap(ret);}
  else {
    return "xx"; //x files: unexplainable phenomena 
  }
} 


/* findBucket function generates the pathname of a bucket file 
   based on its name
*/
char* findBucket(char* bucket_name) {
  char * ret = malloc(strlen(BUCKET_LOCATION) + 4);
  strcpy(ret, BUCKET_LOCATION);
  strcat(ret, "/");
  strcat(ret, bucket_name);
  return ret;
}


/* findBucketRegion function generates the region number where 
   a certain bucket belongs to based on the first character 
   of the bucket name
*/
int findBucketRegion(char* bucket) {
  char indexable[3];
  strcpy(indexable, bucket);
  int first_char = indexable[0];
  if(isdigit(first_char)) { //if first_char is a number, bucket region is 0
    return 0;
  } 
  else if(isalpha(first_char)) {
    //if first_char is an alphabet, assign bucket region using the following
    //algorithm
    int region = (first_char%(BUCKET_REGIONS-2))+1;
    return region;
  }
  else {
    //else, following is the bucket region number
    return BUCKET_REGIONS-1;
  }

}
