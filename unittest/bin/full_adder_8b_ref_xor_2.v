module full_adder_8b_ref (in_a_0, in_a_1, in_a_2, in_a_3, in_a_4, in_a_5, in_a_6, in_a_7, in_b_0, in_b_1, in_b_2, in_b_3, in_b_4, in_b_5, in_b_6, in_b_7, cin, sum__0, sum__1, sum__2, sum__3, sum__4, sum__5, sum__6, sum__7, cout );
   input in_a_0, in_a_1, in_a_2, in_a_3, in_a_4, in_a_5, in_a_6, in_a_7, in_b_0, in_b_1, in_b_2, in_b_3, in_b_4, in_b_5, in_b_6, in_b_7, cin;
   output sum__0, sum__1, sum__2, sum__3, sum__4, sum__5, sum__6, sum__7, cout;
   wire   carry__0, carry__1, carry__2, carry__3, carry__4, carry__5, carry__6;

   assign carry__0 = (in_a_0 & in_b_0) | (cin & (in_a_0 ^ in_b_0));
   assign sum__0 = (in_a_0 ^ in_b_0 ^ cin);

   assign carry__1 = (in_a_1 & in_b_1) | (carry__0 & (in_a_1 ^ in_b_1));
   assign sum__1 = in_a_1 ^ in_b_1 ^ carry__0;

   assign carry__2 = (in_a_2 & in_b_2) | (carry__1 & (in_a_2 ^ in_b_2));
   assign sum__2 = in_a_2 ^ in_b_2 ^ carry__1;
   
   assign carry__3 = (in_a_3 & in_b_3) | (carry__2 & (in_a_3 ^ in_b_3));
   assign sum__3 = in_a_3 ^ in_b_3 ^ carry__2;

   assign carry__4 = (in_a_4 & in_b_4) | (carry__3 & (in_a_4 ^ in_b_4));
   assign sum__4 = in_a_4 ^ in_b_4 ^ carry__3 ;

   assign carry__5 = (in_a_5 & in_b_5) | (carry__4 & (in_a_5 ^ in_b_5));
   assign sum__5 = in_a_5 ^ in_b_5 ^ carry__4;

   assign carry__6 = (in_a_6 & in_b_6) | (carry__5 & (in_a_6 ^ in_b_6));
   assign sum__6 = in_a_6 ^ in_b_6 ^ carry__5;

   assign cout = (in_a_7 & in_b_7) | (carry__6 & (in_a_7 ^ in_b_7));
   assign sum__7 = in_a_7 ^ in_b_7 ^ carry__6;

endmodule   
