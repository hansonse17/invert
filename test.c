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
#include <ctype.h>
#define BUCKET_LOCATION "./buckets"
#define BUCKET_REGIONS 7

int findBucketRegion (char* bucket) {
  char indexable[3];
  strcpy(indexable, bucket);
  //non-letter case = region 0
  //xx case = region BUCKET_REGIONS-1
  int first_char = indexable[0];
  if(isdigit(first_char)) { //48 <= first_char && first_char <= 57) {
    return 0;
  } 
  else if(isalpha(first_char)) { //65 <= indexable[0] && indexable[0] <= 70) {
    return (first_char % (BUCKET_REGIONS-2))+1;
  }
  else {
    return BUCKET_REGIONS-1;
  } /*
  else if(71 <= indexable[0] && indexable[0] <= 75) {
    return 2;
  }
  else if(76 <= indexable[0] && indexable[0] <= 80) {
    return 3;
  }
  else if(81 <= indexable[0] && indexable[0] <= 85) {
    return 4;
  }
  else if(86 <= indexable[0] && indexable[0] <= 90) {
    return 5;
  } else {
    return 6;
    }*/
  
 
    //printf("Here : %d\n", (indexable[0] % (BUCKET_REGIONS-2)) + 1);

  //how did you get here??
  return -1;

}


void main(){
  printf("%d\n", findBucketRegion("666"));
  printf("%d\n", findBucketRegion("AB"));
  printf("%d\n", findBucketRegion("Cc"));
  printf("%d\n", findBucketRegion("DD"));
  printf("%d\n", findBucketRegion("ZY"));
  printf("%d\n", findBucketRegion("&Y"));
}
