#include<stdio.h>

int main (){
  printf("star of trivia\n");
  printf("1) what is the capital of India\n");
  printf("a) New Delhi\n");
  printf("b) Old Delhi\n");
  printf("c) Mumbai\n");
  
  char ans;
  scanf("%c",ans);
  
  if(ans == 'a'){
    printf("correct answer");
  }
  else if (ans == 'b'){
    printf("are you an idiot");
  }
  else{
    printf("no bro that is the financial capital of india");
  }
  return 0;
}
