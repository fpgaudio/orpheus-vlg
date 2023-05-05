`default_nettype none

module mod_sinesource
  ( output var signed [31:0] o_sine
  , input var signed [31:0] i_time
  , input var signed [31:0] i_period
  );

  var bit signed [31:0] angle32;
  assign angle32 = (32768 * i_time) / 32'(i_period);
  var bit signed [15:0] angle16;
  assign angle16 = 16'(angle32);

  var bit signed [15:0] raw_sine;
  mod_sine16 u_sine(raw_sine, angle16);

  assign o_sine = raw_sine * 8;
endmodule
