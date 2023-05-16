`default_nettype none

module mod_attenuator
  ( output logic signed [31:0] o_attenuated // The attenuated value 18.14
  , output logic o_ready // Whether the output is ready
  , input logic unsigned [31:0] i_raw // The input value 18.14
  , input logic unsigned [31:0] i_attenfactor // The attenuation factor 18.14
  , input logic i_trigger // Whether to trigger an attenuation
  , input logic i_clk // The clock signal
  );

  mod_fixed_mul#(32, 14) u_multiplier
    ( o_attenuated, o_ready
    , i_raw, i_attenfactor
    , i_trigger, i_clk
    );
endmodule
