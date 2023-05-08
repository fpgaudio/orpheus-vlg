`default_nettype none

module mod_clock
  ( output logic signed [31:0] o_audio_monotonic
  , output logic o_uart_pulse
  , input logic i_crystal
  );
endmodule
