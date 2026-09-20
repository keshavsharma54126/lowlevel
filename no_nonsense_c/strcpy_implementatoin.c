#include <stdio.h>
#include <string.h>

char *our_strcat(char *dest, const char *src) {
  int n;
  k;
}

char *our_strcpy(char *dest, const char *src) {}
char *our_memset(char) {}
int main(int argc, char *argv[]) {
  printf("hello world \n");
  // first we will take some input string from the command line parms
  if (argc != 2) {
    printf("incorrect no of arguments");
    return 1;
  }
  char dest[1024] = "yo buddy";
  char src[1024] = "";
  for (int i = 1; i < argc; i++) {
    strcat(src, argv[i]);
  }
  printf("src = %s \n", src);
  printf("src = %s \n", dest);
  strcpy(dest, src);
  printf("dest= %s \n", dest);
  printf("\n");
  return 0;
}
