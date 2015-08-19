`timescale 100ps/1ns 

`include "defines.v"

module grid8_test; 

typedef struct packed{ 
	int unsigned address; 
	int unsigned data; 

} NetworkFlit; 


export "DPI-C" function export_func; 
//export "DPI-C" function simulation_time; 
import "DPI-C" context function void import_func(); 
//import "DPI-C" function void init(); 
import "DPI-C" function void terminate(input chandle); 
import "DPI-C" function int message_generation_period(); 
import "DPI-C" function chandle get_tm(); 
import "DPI-C" function int has_packets_in_queue(input chandle tm, input int index,  longint run_time); 
import "DPI-C" function int simulation_complete(input chandle);
import "DPI-C" function void generate_messages(input chandle, input  longint run_time); 
import "DPI-C" function void add_packet_to_network(input chandle, input int index, input  longint run_time, output NetworkFlit ns); 
import "DPI-C" function void remove_packet_from_network(input chandle, input  longint run_time, input int data, input int index); // the data field is used to encode the uid to track flit delay on network
import "DPI-C" string_sv2c=task string_sv2c(string str);

import "DPI-C" function void init (string log_filename, string error_filename, string results_filename, string sim_id, string topology, string trfic_ptrn, input int injection_rate,
  input int message_size, input int messages_in_sim, input int messages_in_warm, input int messages_in_meas, input int short_hop_cost, input int long_hop_cost, int route_mode);

chandle tm_inst;

/*
function longint simulation_time();
 return $time;
endfunction
*/

function void export_func(input int arr[3]); 


endfunction 
integer vcdstart;
	reg rst = 1'b1;;
  reg [(`Grid_Size * `Activity_data) - 1: 0] activity_reg; 
	reg [((`Grid_Size * `Flit_Width)-1):0] din_C;
	wire [((`Grid_Size * `Flit_Width)-1):0] dout_C;
	wire [`Grid_Size-1:0] lr_C, ra_C;
	reg [`Grid_Size-1:0] lr_C_updates, ra_C_updates; // added additional vectors to keep track of handshake state, but also allow controllers time to stabalize
	wire [`Grid_Size-1:0] rr_C, la_C;
	wire [`Grid_Size-1:0] rr_C_delay;


  //Passed Parameters
  string log_file;
  string error_file;
  string results_file;
  string sim_id;
  string topology;
  string traffic_pattern;
  int injection_rate = 2;
  int message_size = 10;
  int total_messages_in_simulation;
  int messages_in_warmup;
  int messages_in_measurement;
  int short_hop_cost;
  int long_hop_cost;
  int max_runtime = 100000000;
  int route_mode = 0;
  bit inited = 1'b0;
  longint last_removed_time = 0;
  longint between_removes_timeout = 15000000; 
  reg [31:0] addr_temp;

 
task add_data_to_network (); // should pass the lr_C_updates and din as argument instead of modifying global variables (sorry future readers)
  NetworkFlit ns;
  for (int i = 0; i < `Grid_Size; i++) begin
    if((la_C[i] == 1'b0 && lr_C_updates[i] != 1'b1 && lr_C[i] !=1'b1) && (has_packets_in_queue(tm_inst, i, $time) != 0)) begin
      add_packet_to_network(tm_inst, i, $time, ns);
      addr_temp = ns.address;
      din_C[(((i+1) * `Flit_Width) - 1) -: `Address_Width] = ns.address[31 -: `Address_Width];
      din_C[((((i+1) * `Flit_Width) - 1) - `Address_Width) -: ((`Uid_data + `Activity_data))] = {ns.data[31 -: `Uid_data], activity_reg[i*`Activity_data +: `Activity_data]};
      activity_reg[i*`Activity_data +: `Activity_data] = ~activity_reg[i*`Activity_data +: `Activity_data];
      lr_C_updates[i] = 1'b1;
      $display("Adding flit with address: 0x%08H and data: %H at time: %f",ns.address, ns.data, $time);
      $display("data on bus: addr 0x%05H data: %H" , din_C[(((i+1) * `Flit_Width) - 1) -: `Address_Width],din_C[((((i+1) * `Flit_Width) - 1) - `Address_Width) -: ((`Uid_data + `Activity_data))]);
      $display("MSB %H, 1MSB: %H, 2MSB: %H, 3MSB: %H", ns.address[31:24], ns.address[23:16], ns.address[15:8], ns.address[7:0]);
      $display("Bounds addr upper: %d, addr lower: %d, data_upper: %d, data_lower: %d",((i+1) * `Flit_Width) - 1, ((i+1) * `Flit_Width) - 1 - `Address_Width, ((((i+1) * `Flit_Width) - 1) - `Address_Width), ((((i+1) * `Flit_Width) - 1) - `Address_Width) - ((`Uid_data + `Activity_data)));  
      $display("Concatonated data %H", {ns.address[31 -: `Address_Width],{ns.data[31 -: `Uid_data], activity_reg[i*`Activity_data +: `Activity_data]}}); 
  end else if ((la_C[i] == 1'b1 && lr_C[i] == 1'b1) && lr_C_updates[i] == 1'b1) begin
      lr_C_updates[i] = 1'b0;
      //$display("Setting lr_C_updates to 0");
		
    end else begin
			//$display("ELSE case!");
		end
  end 
endtask


task remove_data_from_network (); // should pass the lr_C_updates and din as argument instead of modifying global variables (sorry future readers)
  NetworkFlit ns;
  for (int i = 0; i < `Grid_Size; i++) begin
    if((rr_C_delay[i] == 1'b1) && (ra_C_updates[i] != 1'b1) && (ra_C[i] != 1'b1)) begin
      int uid;
      uid = dout_C[((((i+1) * `Flit_Width) - 1) - `Address_Width) -: `Uid_data ];      
      remove_packet_from_network(tm_inst, $time, uid, i);
      ra_C_updates[i] = 1'b1;
      //$display("removing flit with uid: %d", uid);
      last_removed_time = $time;
    end else if((rr_C_delay[i] == 1'b0) && (ra_C[i] == 1'b1) && (ra_C_updates[i] == 1'b1)) begin
      ra_C_updates[i] = 1'b0;
      //$display("Completing Cycle at index  %d", i);
    end else begin
    
    end
  end
endtask

int generate_delay = message_generation_period();

always begin
  #(generate_delay);
  if(inited == 1'b1) begin
    generate_messages(tm_inst, $time);
    add_data_to_network();
  end
end 

always @(la_C) begin
  //if(inited  == 1'b1) begin
    add_data_to_network();
  //end else begin
  //  #3;
  //end
end

always @(rr_C_delay) begin
  //if(inited == 1'b1) begin
    remove_data_from_network();
  //end else begin
  //  #3;
  // end
end


grid8 Grid8( 
  .rst(rst), 
  .rr_C(rr_C), 
  .la_C(la_C), 
  .dout_C(dout_C), 
  .lr_C(lr_C), 
  .ra_C(ra_C), 
  .din_C(din_C));

 assign #(2) lr_C = lr_C_updates;
 assign  ra_C = ra_C_updates;
 assign #(1) rr_C_delay = rr_C;

string str;

initial 
begin

// Ugh Getting params from the command line isn't pretty

   if($value$plusargs("log_file=%s", log_file)) begin
    $display("Log Filename %0s", log_file);
   end else begin
    $display("Warning Log Not provided using \"network_sim.log\"");
    log_file = "netowrk_sim.log";
   end

   if($value$plusargs("error_file=%s", error_file)) begin
    $display("Error Filename %0s", error_file);
   end else begin
    $display("Warning Error File not provided using \"network_sim_error.err\"");
    error_file = "network_sim_error.err";
   end

   if($value$plusargs("results_file=%s", results_file)) begin
    $display("Results Filename %0s", results_file);
   end else begin
    $display("Warning Results File not provided, using: \"network_sim_results.txt\"");
    results_file = "network_sim_results.txt";
   end

   if($value$plusargs("sim_id=%s", sim_id)) begin
    $display("simulation_id %0s", sim_id);
   end else begin
    sim_id = "torus_random_10";
   end  
  
   if($value$plusargs("topology=%s", topology)) begin
    $display("topology %0s", topology);
   end else begin
    topology = "torus";
   end

   if($value$plusargs("traf_ptrn=%s", traffic_pattern)) begin
    $display("Traffic %0s", traffic_pattern);
   end else begin
    traffic_pattern = "random";
   end

   if($value$plusargs("inj_rate=%d", injection_rate)) begin
    $display("Injection_rate %d", injection_rate);
   end else begin
    $display("Injection rate not provided, using: 8");
    injection_rate = 8; 
  end 

  if($value$plusargs("msg_size=%d", message_size)) begin
    $display("Message size: %d", message_size);
   end else begin
    $display("Message size not provided, using: 8");
    message_size= 8; 
  end 


  if($value$plusargs("msg_in_sim=%d", total_messages_in_simulation)) begin
    $display("Message in simulation: %d", total_messages_in_simulation);
   end else begin
    $display("Message in Simulation not provided, using: 100");
    total_messages_in_simulation = 100;
  end 

 if($value$plusargs("msg_in_wrm=%d", messages_in_warmup)) begin
    $display("Message in warmup: %d", messages_in_warmup);
   end else begin
    $display("Message in warmup not provided, using: 20");
    messages_in_warmup = 20;
  end  

 if($value$plusargs("msg_in_mes=%d", messages_in_measurement)) begin
    $display("Message in measurement: %d", messages_in_measurement);
   end else begin
    $display("Message in measure not provided, using: 60");
    messages_in_measurement = 60;
  end  

if($value$plusargs("shop_cst=%d", short_hop_cost)) begin
    $display("Short Hop Cost: %d", short_hop_cost);
   end else begin
    $display("Short Hop Cost Not provided, using: 1");
    short_hop_cost= 1;
  end  

if($value$plusargs("lhop_cst=%d", long_hop_cost)) begin
    $display("Long Hop Cost: %d", long_hop_cost);
   end else begin
    $display("Long Hop Cost Not provided, using: 1");
    long_hop_cost= 1;
  end  

if($value$plusargs("route_mode=%d", route_mode)) begin
    $display("route_mode: %d", route_mode);
   end else begin
    $display("Route_mode not provided using: 0");
    long_hop_cost= 1;
  end  

// phewww done with that


init(log_file, error_file, results_file, sim_id, topology, traffic_pattern, injection_rate, message_size, total_messages_in_simulation, messages_in_warmup, 
     messages_in_measurement, short_hop_cost, long_hop_cost, route_mode); // MUST BE CALLED BEFORE ANYTHING ELSE


tm_inst = get_tm(); // holds the state of the entire simulation
  
activity_reg = {(`Grid_Size * `Activity_data / 2){2'b10}};
  
  din_C = {(`Grid_Size*`Flit_Width){0}};
  lr_C_updates = {`Grid_Size{0}};
  ra_C_updates = {`Grid_Size{0}};
  rst = 1'b1;
  #20;
  rst = 1'b0;
  #20;
  //vcd Start
	$dumpfile("grid8.vcd");
	$dumpvars(0, "Grid8");
	$dumpon;
	vcdstart= $time;
	$display("Starting vcd dump at sim time %.3f\n", $time);

  inited = 1'b1;
  $display("Starting Loop\n");
	generate_messages(tm_inst, $time); 
  do begin
 
    //add_data_to_network();
    //remove_data_from_network();
    #1;
    //$display("Simulation Complete Returned  %d\n", simulation_complete());
  end while((!simulation_complete(tm_inst)) && ($time <= max_runtime)); // && (($time - last_removed_time) < between_removes_timeout));
	if (simulation_complete(tm_inst)) begin
		$display("Traffic Managers says simulation is complete");
	end else if(($time - last_removed_time) >= between_removes_timeout) begin
    $display("Simulation timeout at");
  end else begin
		$display("Traffic Managers says simulation is NOT complete");
  end
  terminate(tm_inst);
	$display("FULL VCD run time:%.3f", last_removed_time - vcdstart);
	$dumpoff;
	$dumpflush;
  $stop;
end 


final begin
		$display("Calling Final Method");
end

endmodule
