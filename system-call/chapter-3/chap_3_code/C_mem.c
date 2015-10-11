/*************************************************************************
	> File Name: C_mem.c
	> Author: 
	> Mail: 
	> Created Time: 2015年10月11日 星期日 09时57分20秒
 ************************************************************************/

#include<stdio.h>

int global_one = 100;
int global_two ;

int print(int i){

    printf("the data is %d \n",i);

    return 0;

}


int main(){

    static int local_one = 1;
    
    int local_two ;

    print(local_one);

    return 0;

}
