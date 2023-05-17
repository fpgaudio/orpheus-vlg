module mod_7seg#
  ( parameter DEFAULT_STATE = 'b0000000 // The default state.
  ) ( output logic [0:6] o_segments // The individual segments,
                                  // clockwise, top first.
  , input logic [3:0] i_value // Input value
  , input logic i_clk // The clock.
  , input logic i_nrst // Negative async reset.
  );

  always @(posedge i_clk or negedge i_nrst)
    if (!i_nrst) begin
      o_segments <= ~DEFAULT_STATE;
    end else begin
      case (i_value)
        0: begin
          o_segments <= ~'b1111110;
        end
        1: begin
          o_segments <= ~'b0110000;
        end
        2: begin
          o_segments <= ~'b1101101;
        end
        3: begin
          o_segments <= ~'b1111001;
        end
        4: begin
          o_segments <= ~'b0110011;
        end
        5: begin
          o_segments <= ~'b1011011;
        end
        6: begin
          o_segments <= ~'b1011111;
        end
        7: begin
          o_segments <= ~'b1110000;
        end
        8: begin
          o_segments <= ~'b1111111;
        end
        9: begin
          o_segments <= ~'b1111011;
        end
        'ha: begin
          o_segments <= ~'b1110111;
        end
        'hb: begin
          o_segments <= ~'b0011111;
        end
        'hc: begin
          o_segments <= ~'b1001110;
        end
        'hd: begin
          o_segments <= ~'b0111101;
        end
        'he: begin
          o_segments <= ~'b1001111;
        end
        'hf: begin
          o_segments <= ~'b1000111;
        end
        default: begin
          o_segments <= ~DEFAULT_STATE;
        end
      endcase
    end
endmodule
