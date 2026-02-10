#include<stdio.h>


int area_triangle(float height,float base){
  float area  = 0.5 *(base)*(height);
  return area;
}
int main (){
 //lets creat the area of the circle 
  float height,base=0.00;
  printf("Height in meters");
  fflush(stdout);
  scanf("%f",&height);
  printf("Base in meters");
  fflush(stdout);
  scanf("%f",&base);

  float area = area_triangle(height,base);
  printf("area of the triangle is %f",area);
  
  return 0;
}
