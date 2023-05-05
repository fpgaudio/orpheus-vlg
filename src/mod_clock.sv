`default_nettype none

module mod_clock
  ( output var signed [31:0] o_audio_monotonic
  , output var o_uart_pulse
  , input var i_crystal
  );
endmodule
