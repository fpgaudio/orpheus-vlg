module mod_fixed_mul#
  ( parameter INPUT_WIDTH = 32 // The underlying type
  , parameter INPUT_POINT = 8 // The number of bits the mantissa consumes
  )
  ( output logic [(INPUT_WIDTH - 1):0] o_out // The output value
  , output logic o_ready // Whether the output value is ready

  , input logic [(INPUT_WIDTH - 1):0] i_a // An input multiplicand
  , input logic [(INPUT_WIDTH - 1):0] i_b // An input multiplicand
  , input logic i_trigger // Whether to trigger the calculation
  , input logic i_clk // The clock
  );

  logic [((INPUT_WIDTH * 2) - 1):0] temp_reg;
  assign temp_reg = i_a * i_b;

  assign o_out = INPUT_WIDTH'(temp_reg >> INPUT_POINT);
  assign o_ready = 1; // Combi implementation
endmodule
