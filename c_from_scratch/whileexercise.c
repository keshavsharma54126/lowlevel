#include<stdio.h>

int main (){
  int x = 10;
  while (x>-1){
    if (x==0){
      printf("boom boom");
      return 0;
    }
    printf("tick tick %d\n",x);
    x-=1;
  }
  return 0;
}
