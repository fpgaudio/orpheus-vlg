module orpheus_sine16(
    output signed [15:0] o_sine,
    input signed [15:0] i_angle
);
  wire signed [15:0] angle;
  wire [0:0] carry;

  assign angle = i_angle << 1;
  assign carry = angle < 0;

  wire signed [15:0] angleBound;
  assign angleBound = (angle == (angle | 'h4000)) ? ('hffff - angle) : (angle);
  wire signed [15:0] angleBound2;
  assign angleBound2 = (angleBound & 'h7fff) >> 1;

  wire unsigned [31:0] angleU;
  assign angleU = 32'(angleBound2);

  //// Implement the formula
  //// y * 2^-n * ( A1 - 2^(q-p)* y * 2^-n * y * 2^-n * [B1 - 2^-r * y * 2^-n * C1
  //// * y]) * 2^(a-q)
  const integer coeffA1 = 3370945099;
  const integer coeffB1 = 2746362156;
  const integer coeffC1 = 292421;
  const integer coeffN = 13;
  const integer coeffR = 3;
  const integer coeffP = 32;
  const integer coeffQ = 31;
  const integer coeffA = 12;

  // Ugly, but optimizable
  wire unsigned [31:0] outU0;
  wire unsigned [31:0] outU1;
  wire unsigned [31:0] outU2;
  wire unsigned [31:0] outU3;
  wire unsigned [31:0] outU4;
  wire unsigned [31:0] outU5;
  wire unsigned [31:0] outU6;
  assign outU0 = (coeffC1 * angleU) >> coeffN;
  assign outU1 = coeffB1 - ((angleU * outU0) >> coeffR);
  assign outU2 = angleU * (outU1 >> coeffN);
  assign outU3 = angleU * (outU2 >> coeffN);
  assign outU4 = coeffA1 - (outU3 >> (coeffP - coeffQ));
  assign outU5 = angleU * (outU4 >> coeffN);
  assign outU6 = (outU5 + (1 << (coeffQ - coeffA - 1))) >> (coeffQ - coeffA);

  assign o_sine = carry ? -(16'(outU6)) : (16'(outU6));
endmodule
