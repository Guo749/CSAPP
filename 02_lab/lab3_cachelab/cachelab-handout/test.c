#include <stdio.h>
#include <stdlib.h>

int main(){
	size_t sz = 0;
  ssize_t len = 0;
  char * line = NULL;

  FILE *f = fopen("names.txt", "r");
  /*
  		如果 getline 拿到的字符串是 bread
  		那么len的长度为6, 为 b r e a d \n
  		事实长度为7. 后面多一个\0
  */
  while((len = getline(&line, &sz, f)) >= 0){
  		printf("%s", line);
  }

  free(line);
  return EXIT_SUCCESS;
}
