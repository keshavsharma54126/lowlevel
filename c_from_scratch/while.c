#include<stdio.h>

int main (){
  int x;
//this is one way to do thisk
  while(1){
    printf("please enter your number \n");
    scanf("%d",&x);
    if (x==1){
      break;
    }
  }
  printf("Important \n");
  return 0;
}
