module mod_sinesource
  ( output var signed [31:0] o_sine
  , input var unsigned [63:0] i_time
  , input var unsigned [31:0] i_period
  );

  localparam MAX_I16 = 32767;

  var bit unsigned [63:0] angle64;
  assign angle64 = (MAX_I16 * i_time) / 64'(i_period);
  var bit signed [15:0] angle16;
  assign angle16 = 16'(angle64 % MAX_I16);

  var bit signed [15:0] raw_sine;
  mod_sine16 u_sine(raw_sine, angle16);

  assign o_sine = raw_sine * 8;
endmodule
