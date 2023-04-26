
module mod_orpheus_sine_source(
    output var [31:0] o_source,
    input var [15:0] i_angle
);
    wire signed [15:0] small_sine;
    orpheus_sine16 u_sine16(small_sine, i_angle);

    assign o_source = 32'(small_sine) << 3;
endmodule
