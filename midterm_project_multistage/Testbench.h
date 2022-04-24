#ifndef TESTBENCH_H_
#define TESTBENCH_H_

#include <string>
using namespace std;

#include <systemc>
using namespace sc_core;

#ifndef NATIVE_SYSTEMC
#include <cynw_p2p.h>
#endif

#include "filter_def.h"

class Testbench : public sc_module {
	public:
		sc_in_clk i_clk;
		sc_out < bool >  o_rst;
#ifndef NATIVE_SYSTEMC
		cynw_p2p< sc_dt::sc_biguint<128> >::base_out o_array;
		cynw_p2p< sc_dt::sc_biguint<128> >::base_in i_result;
#else
		sc_fifo_out< sc_dt::sc_biguint<128> > o_array;
		sc_fifo_in< sc_dt::sc_biguint<128> > i_result;
#endif

		SC_HAS_PROCESS(Testbench);

		Testbench(sc_module_name n);
		~Testbench();

		int read_array(string infile_name);
		
	private:
		unsigned int A[5][16];
		unsigned int n_txn;
		sc_time max_txn_time;
		sc_time min_txn_time;
		sc_time total_txn_time;
		sc_time total_start_time;
		sc_time total_run_time;

		void feed_array();
		void fetch_result();
};
#endif
