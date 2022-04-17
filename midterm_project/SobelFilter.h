#ifndef SOBEL_FILTER_H_
#define SOBEL_FILTER_H_
#include <systemc>
using namespace sc_core;

#ifndef NATIVE_SYSTEMC
#include <cynw_p2p.h>
#endif

#include "filter_def.h"

class SobelFilter: public sc_module
{
public:
	sc_in_clk i_clk;
	sc_in < bool >  i_rst;
#ifndef NATIVE_SYSTEMC
	cynw_p2p< sc_dt::sc_biguint<128> >::in i_rgb;
	cynw_p2p< sc_dt::sc_biguint<128> >::out o_result;
#else
	sc_fifo_in< sc_dt::sc_biguint<128> > i_rgb;
	sc_fifo_out< sc_dt::sc_biguint<128> > o_result;
#endif

	//void merge(int first, int second, int width);
	SC_HAS_PROCESS( SobelFilter );
	SobelFilter( sc_module_name n );
	~SobelFilter();
private:
	void do_filter();
	sc_dt::sc_uint<8> A[16], C[16];
};
#endif
