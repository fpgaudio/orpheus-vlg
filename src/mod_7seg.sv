module mod_7seg#
  ( parameter DEFAULT_STATE = 7'b0000000 // The default state.
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
        4'h0: begin
          o_segments <= ~7'b1111110;
        end
        4'h1: begin
          o_segments <= ~7'b0110000;
        end
        4'h2: begin
          o_segments <= ~7'b1101101;
        end
        4'h3: begin
          o_segments <= ~7'b1111001;
        end
        4'h4: begin
          o_segments <= ~7'b0110011;
        end
        4'h5: begin
          o_segments <= ~7'b1011011;
        end
        4'h6: begin
          o_segments <= ~7'b1011111;
        end
        4'h7: begin
          o_segments <= ~7'b1110000;
        end
        4'h8: begin
          o_segments <= ~7'b1111111;
        end
        4'h9: begin
          o_segments <= ~7'b1111011;
        end
        4'ha: begin
          o_segments <= ~7'b1110111;
        end
        4'hb: begin
          o_segments <= ~7'b0011111;
        end
        4'hc: begin
          o_segments <= ~7'b1001110;
        end
        4'hd: begin
          o_segments <= ~7'b0111101;
        end
        4'he: begin
          o_segments <= ~7'b1001111;
        end
        4'hf: begin
          o_segments <= ~7'b1000111;
        end
        default: begin
          o_segments <= ~DEFAULT_STATE;
        end
      endcase
    end
endmodule
