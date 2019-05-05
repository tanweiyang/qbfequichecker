module full_adder_8b_ref (in_a_0, in_a_1, in_a_2, in_a_3, in_a_4, in_a_5, in_a_6, in_a_7, in_b_0, in_b_1, in_b_2, in_b_3, in_b_4, in_b_5, in_b_6, in_b_7, cin, sum_0, sum_1, sum_2, sum_3, sum_4, sum_5, sum_6, sum_7, cout );
   input in_a_0, in_a_1, in_a_2, in_a_3, in_a_4, in_a_5, in_a_6, in_a_7, in_b_0, in_b_1, in_b_2, in_b_3, in_b_4, in_b_5, in_b_6, in_b_7, cin;
   output sum_0, sum_1, sum_2, sum_3, sum_4, sum_5, sum_6, sum_7, cout;
   wire   carry_0, carry_1, carry_2, carry_3, carry_4, carry_5, carry_6;

   assign carry_0 = (in_a_0 & in_b_0) | (cin & (in_a_0 ^ in_b_0));
   assign sum_0 = (in_a_0 ^ in_b_0 ^ cin);

   assign carry_1 = (in_a_1 & in_b_1) | (carry_0 & (in_a_1 ^ in_b_1));
   assign sum_1 = in_a_1 ^ in_b_1 ^ carry_0;

   assign carry_2 = (in_a_2 & in_b_2) | (carry_1 & (in_a_2 ^ in_b_2));
   assign sum_2 = in_a_2 ^ in_b_2 ^ carry_1;
   
   assign carry_3 = (in_a_3 & in_b_3) | (carry_2 & (in_a_3 ^ in_b_3));
   assign sum_3 = in_a_3 ^ in_b_3 ^ carry_2;

   assign carry_4 = (in_a_4 & in_b_4) | (carry_3 & (in_a_4 ^ in_b_4));
   assign sum_4 = in_a_4 ^ in_b_4 ^ carry_3 ;

   assign carry_5 = (in_a_5 & in_b_5) | (carry_4 & (in_a_5 ^ in_b_5));
   assign sum_5 = in_a_5 ^ in_b_5 ^ carry_4;

   assign carry_6 = (in_a_6 & in_b_6) | (carry_5 & (in_a_6 ^ in_b_6));
   assign sum_6 = in_a_6 ^ in_b_6 ^ carry_5;

   assign cout = (in_a_7 & in_b_7) | (carry_6 & (in_a_7 ^ in_b_7));
   assign sum_7 = in_a_7 ^ in_b_7 ^ carry_6;

endmodule   
