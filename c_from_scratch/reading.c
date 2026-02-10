// so we are going to read some 
// variable as input from the comand line 
// and do a greeting with it 
//

#include<stdio.h> 

int main(){
  char name[16];
  printf("Hello what is your name? \n");
  scanf("%s",name);

  printf("Hello %s, welocme to this tutorial",name);
  return 0;
}
