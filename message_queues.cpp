#include "stdio.h" 
#include "svdpi.h" 
#include <stdint.h>

extern "C" { 


extern void export_func(svBitVec32 x[3] ); 
extern int simulation_time();

void import_func() { 
Flit s_data; 
unsigned int arr[3]; 

s_data.a = 51; 
s_data.b = 242; 
s_data.c = 35; 

printf( "C : s_data.a = %d\n", s_data.a ); 
printf( "C : s_data.b = %d\n", s_data.b ); 
printf( "C : s_data.c = %d\n\n", s_data.c ); 
printf( "Simulation Time = %d\n\n", simulation_time());

arr[0] = s_data.a ; 
arr[1] = s_data.b ; 
arr[2] = s_data.c ; 

export_func(arr); 

} 
} 

