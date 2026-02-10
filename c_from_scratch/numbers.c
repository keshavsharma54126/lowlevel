#include<stdio.h> 

int main(){
  int num1,num2;
  printf("what is your first number please \n");
  scanf("%d",&num1);
  printf("what is your second number \n");
  scanf("%d",&num2);
  
  int sum = num1 + num2 ;

  printf("sum of the two numbers is %d",sum);
  return 0;

}
