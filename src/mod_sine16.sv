`default_nettype none

module mod_sine16
  (output var signed [15:0] o_sine
  , input var signed [15:0] i_angle
  );

  var bit signed [15:0] angle;
  logic [0:0] carry;

  assign angle = i_angle << 1;
  assign carry = angle < 0;

  var bit signed [15:0] angleBound;
  assign angleBound = (angle == (angle | 'h4000)) ? ('hffff - angle) : (angle);
  var bit signed [15:0] angleBound2;
  assign angleBound2 = (angleBound & 'h7fff) >> 1;

  var bit unsigned [31:0] angleU;
  assign angleU = 32'(angleBound2);

  //// Implement the formula
  //// y * 2^-n * ( A1 - 2^(q-p)* y * 2^-n * y * 2^-n * [B1 - 2^-r * y * 2^-n * C1
  //// * y]) * 2^(a-q)
  const bit [31:0] coeffA1 = 3370945099;
  const bit [31:0] coeffB1 = 2746362156;
  const bit [31:0] coeffC1 = 292421;
  const bit [31:0] coeffN = 13;
  const bit [31:0] coeffR = 3;
  const bit [31:0] coeffP = 32;
  const bit [31:0] coeffQ = 31;
  const bit [31:0] coeffA = 12;

  // Ugly, but optimizable
  var bit unsigned [31:0] outU0;
  var bit unsigned [31:0] outU1;
  var bit unsigned [31:0] outU2;
  var bit unsigned [31:0] outU3;
  var bit unsigned [31:0] outU4;
  var bit unsigned [31:0] outU5;
  var bit unsigned [31:0] outU6;
  assign outU0 = (coeffC1 * angleU) >> coeffN;
  assign outU1 = coeffB1 - ((angleU * outU0) >> coeffR);
  assign outU2 = angleU * (outU1 >> coeffN);
  assign outU3 = angleU * (outU2 >> coeffN);
  assign outU4 = coeffA1 - (outU3 >> (coeffP - coeffQ));
  assign outU5 = angleU * (outU4 >> coeffN);
  assign outU6 = (outU5 + (1 << (coeffQ - coeffA - 1))) >> (coeffQ - coeffA);

  assign o_sine = carry ? -(16'(outU6)) : (16'(outU6));
endmodule
