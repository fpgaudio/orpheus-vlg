module mod_7seg
  ( output logic o_a // Top segment
  , output logic o_b // Top right segment
  , output logic o_c // Bottom right segment
  , output logic o_d // Bottom segment
  , output logic o_e // Bottom left segment
  , output logic o_f // Top left segment
  , output logic o_g // Middle segment
  , input logic [3:0] i_value // Input value
  );
  always @(*) begin
    case (i_value)
      0: begin
        assign o_a = 1;
        assign o_b = 1;
        assign o_c = 1;
        assign o_d = 1;
        assign o_e = 1;
        assign o_f = 1;
        assign o_g = 0;
      end
      1: begin
        assign o_a = 0;
        assign o_b = 1;
        assign o_c = 1;
        assign o_d = 0;
        assign o_e = 0;
        assign o_f = 0;
        assign o_g = 0;
      end
      2: begin
        assign o_a = 1;
        assign o_b = 1;
        assign o_c = 0;
        assign o_d = 1;
        assign o_e = 1;
        assign o_f = 0;
        assign o_g = 1;
      end
      3: begin
        assign o_a = 1;
        assign o_b = 1;
        assign o_c = 1;
        assign o_d = 1;
        assign o_e = 0;
        assign o_f = 0;
        assign o_g = 1;
      end
      4: begin
        assign o_a = 0;
        assign o_b = 1;
        assign o_c = 1;
        assign o_d = 0;
        assign o_e = 0;
        assign o_f = 1;
        assign o_g = 1;
      end
      5: begin
        assign o_a = 1;
        assign o_b = 0;
        assign o_c = 1;
        assign o_d = 1;
        assign o_e = 0;
        assign o_f = 0;
        assign o_g = 1;
      end
      6: begin
        assign o_a = 1;
        assign o_b = 0;
        assign o_c = 1;
        assign o_d = 1;
        assign o_e = 1;
        assign o_f = 1;
        assign o_g = 1;
      end
      7: begin
        assign o_a = 1;
        assign o_b = 1;
        assign o_c = 1;
        assign o_d = 0;
        assign o_e = 0;
        assign o_f = 0;
        assign o_g = 0;
      end
      8: begin
        assign o_a = 1;
        assign o_b = 1;
        assign o_c = 1;
        assign o_d = 1;
        assign o_e = 1;
        assign o_f = 1;
        assign o_g = 1;
      end
      9: begin
        assign o_a = 1;
        assign o_b = 1;
        assign o_c = 1;
        assign o_d = 1;
        assign o_e = 0;
        assign o_f = 1;
        assign o_g = 1;
      end
      default: begin
        assign o_a = 0;
        assign o_b = 0;
        assign o_c = 0;
        assign o_d = 0;
        assign o_e = 0;
        assign o_f = 0;
        assign o_g = 0;
      end
    endcase
  end
endmodule
