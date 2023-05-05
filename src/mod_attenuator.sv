`default_nettype none

module mod_attenuator
  ( output var signed [31:0] o_attenuated
  , input var signed [31:0] i_raw
  , input var signed [15:0] i_attenfactor
  );

  assign o_attenuated = (i_raw * i_attenfactor) / 32768;
endmodule
