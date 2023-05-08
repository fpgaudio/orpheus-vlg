`default_nettype none

module mod_attenuator
  ( output logic signed [31:0] o_attenuated
  , input logic signed [31:0] i_raw
  , input logic signed [15:0] i_attenfactor
  );

  assign o_attenuated = (i_raw * i_attenfactor) / 32767;
endmodule
