module mod_synth
  ( output var signed [31:0] o_sound
  , input var unsigned [63:0] i_time
  , input var signed [15:0] i_atten1
  , input var signed [15:0] i_atten2
  , input var signed [15:0] i_atten3
  , input var signed [15:0] i_atten4
  , input var signed [15:0] i_atten_out
  , input var unsigned [31:0] i_period
  );

  var bit signed [31:0] sine1, sine1_attenuated;
  mod_sinesource u_sine1(sine1, i_time, i_period);
  mod_attenuator u_atten1(sine1_attenuated, sine1, i_atten1);

  var bit signed [31:0] sine2, sine2_attenuated;
  mod_sinesource u_sine2(sine2, i_time, i_period / 2);
  mod_attenuator u_atten2(sine2_attenuated, sine2, i_atten2);

  var bit signed [31:0] sine3, sine3_attenuated;
  mod_sinesource u_sine3(sine3, i_time, i_period / 4);
  mod_attenuator u_atten3(sine3_attenuated, sine3, i_atten3);

  var bit signed [31:0] sine4, sine4_attenuated;
  mod_sinesource u_sine4(sine4, i_time, i_period / 8);
  mod_attenuator u_atten4(sine4_attenuated, sine4, i_atten4);

  mod_attenuator u_out_atten
    ( o_sound
    , sine1_attenuated + sine2_attenuated + sine3_attenuated + sine4_attenuated
    , i_atten_out
    );
endmodule
