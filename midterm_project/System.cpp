#include "System.h"
#include <iostream>
System::System( sc_module_name n, string input_bmp, string output_bmp ): sc_module( n ), 
	tb("tb"), sobel_filter("sobel_filter"), clk("clk", CLOCK_PERIOD, SC_NS), rst("rst"), _output_bmp(output_bmp)
{
	tb.i_clk(clk);
	tb.o_rst(rst);
	sobel_filter.i_clk(clk);
	sobel_filter.i_rst(rst);
	tb.o_array(rgb);
	tb.i_result(result);
	sobel_filter.i_rgb(rgb);
	sobel_filter.o_result(result);

	std::cout << input_bmp << std::endl;
  	tb.read_array(input_bmp);
}

System::~System() {
	;
}
