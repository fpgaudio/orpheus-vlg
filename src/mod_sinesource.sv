module mod_sinesource
  ( output logic signed [31:0] o_sine // The current sine value 18.14
  , output logic o_ready // Pulsed when the signal is ready
  , input logic unsigned [63:0] i_time // The current time
  , input logic unsigned [31:0] i_frequency // The sine frequency in Hz (integer)
  , input logic i_trigger // Pulse When the sampling should trigger.
  , input logic i_clk // The global clock
  , input logic i_nrst // Negative reset.
  );
  localparam TWO_PI = 32'h0001921f;
  localparam SAMPLE_RATE = 48000;
  logic [63:0] pi2scaled;
  assign pi2scaled = i_time * TWO_PI / SAMPLE_RATE * i_frequency ;
  logic [31:0] angle32;
  assign angle32 = (pi2scaled) % TWO_PI;

  logic signed [16:0] sine;
  logic signed [16:0] cosine; // Unused

  // CORDIC needs to be reset every time a new sample comes in.
  logic cordic_reset = 0;
  always @(posedge i_clk)
    cordic_reset = i_trigger;

  cordic u_cordic(i_clk, cordic_reset, angle32, 1, sine, cosine, o_ready);

  // We also need to FF our current state.
  always_ff @(posedge i_clk)
    if (o_ready) begin
      o_sine <= 32'(sine);
    end
endmodule
