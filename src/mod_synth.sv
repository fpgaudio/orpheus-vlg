module mod_synth#
  ( parameter NUM_HARMONICS = 5
  )
  ( output logic signed [31:0] o_sound // The output data.
  , output logic o_ready // Pulsed when the signal is ready.

  , input logic unsigned [63:0] i_time // The current time tick.
  , input logic unsigned [31:0] i_frequency // The base frequency 18.14
  , input logic [31:0] i_atten_harmonics[0:(NUM_HARMONICS - 1)] // The harmonic attenuations 18.14
  , input logic signed [31:0] i_atten_out // The output attenuation 18.14
  , input logic i_trigger // The trigger signal.
  , input logic i_clk // The clock signal
  , input logic i_nrst // Negative reset.
  );

  logic [0:(NUM_HARMONICS - 1)] sine_ready;
  logic [0:(NUM_HARMONICS - 1)] attenuators_ready;
  logic [0:(NUM_HARMONICS - 1)][31:0] sine_outputs;
  logic [0:(NUM_HARMONICS - 1)][31:0] attenuated_sines;
  generate
    for (genvar i = 0; i < NUM_HARMONICS; i++) begin: l_sine_generate
      mod_sinesource u_sine
        ( sine_outputs[i]
        , sine_ready[i]
        , i_time
        , i_frequency * (1 << i) // Double the frequency for each harmonic
        , i_trigger
        , i_clk
        , i_nrst
        );

      // Each sine must be attenuated by the attenuation factor
      mod_attenuator u_atten
        ( attenuated_sines[i]
        , attenuators_ready[i]
        , sine_outputs[i]
        , i_atten_harmonics[i]
        , sine_ready[i] // Should trigger when the sine spits out data.
        , i_clk
        );
    end
  endgenerate

  logic [31:0] sum_of_sines
    = attenuated_sines[0]
    + attenuated_sines[1]
    + attenuated_sines[2]
    + attenuated_sines[3]
    + attenuated_sines[4]
    ;

  logic sines_ready
    = sine_ready[0]
    & sine_ready[1]
    & sine_ready[2]
    & sine_ready[3]
    & sine_ready[4]
    ;

  logic out_atten_rdy;
  mod_attenuator u_atten_out
    ( o_sound
    , out_atten_rdy
    , sum_of_sines
    , i_atten_out
    , i_trigger
    , i_clk
    );

  assign o_ready = sines_ready;
endmodule
