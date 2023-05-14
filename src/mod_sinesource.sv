
module mod_sinesource
  ( output logic signed [31:0] o_sine // The current sine value
  , output logic o_ready // Pulsed when the signal is ready
  , input logic unsigned [63:0] i_time // The current time
  , input logic unsigned [31:0] i_frequency // The sine period.
  , input logic i_trigger // Pulse When the sampling should trigger.
  , input logic i_clk // The global clock
  , input logic i_nrst // Negative reset.
  );
  localparam TWO_PI = 32'h0001921f;
  logic [63:0] pi2scaled = i_time * TWO_PI * i_frequency;
  logic [31:0] angle32 = (pi2scaled) % TWO_PI;

  logic signed [16:0] sine;
  logic signed [16:0] cosine;
  cordic u_cordic(i_clk, !i_nrst, angle32, i_trigger, sine, cosine, o_ready);
  assign o_sine = 32'(sine);
endmodule
