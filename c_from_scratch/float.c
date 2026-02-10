#include<stdio.h> 

int main(){
  float radius,area;
  float pi=3.14;
  printf("please tell me the radius of the circle that you want to calculate the radius for \n");

  scanf("%f",&radius);
  area = (radius*radius*pi);
  printf("the area of the circle is %f",area);


  return 0;

}
