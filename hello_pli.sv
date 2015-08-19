program main; 

export "DPI-C" function export_func; 
import "DPI-C" function void import_func(); 

typedef struct packed{ 
int a; 
int b; 
byte c; 
} SV_struct; 

function void export_func(input int arr[3]); 

SV_struct s_data; 

s_data.a = arr[0]; 
s_data.b = arr[1]; 
s_data.c = arr[2]; 

$display("SV: s_data.a = %0d", s_data.a ); 
$display("SV: s_data.b = %0d", s_data.b ); 
$display("SV: s_data.c = %0d \n", s_data.c ); 
endfunction 

initial 
begin 
import_func(); 
end 

endprogram 