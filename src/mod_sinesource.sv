module mod_sinesource
  ( output logic signed [31:0] o_sine // The current sine value
  , output logic o_ready // Pulsed when the signal is ready
  , input logic unsigned [63:0] i_time // The current time
  , input logic unsigned [31:0] i_period // The sine period.
  , input logic i_clk // The global clock
  , input logic i_nrst // Negative reset.
  );

  localparam MAX_I16 = 32767;

  logic unsigned [63:0] angle64;
  assign angle64 = (MAX_I16 * i_time) / 64'(i_period);
  logic signed [15:0] angle16;
  assign angle16 = 16'(angle64 % MAX_I16);

  logic signed [16:0] sine;
  logic signed [16:0] cosine;
  cordic u_cordic(i_clk, !i_nrst, angle16, 1, sine, cosine, o_ready);
  assign o_sine = 32'(sine);
endmodule
