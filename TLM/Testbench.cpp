#include <cassert>
#include <cstdio>
#include <cstdlib>
using namespace std;

#include "Testbench.h"


Testbench::Testbench(sc_module_name n)
    : sc_module(n), initiator("initiator") {
        SC_THREAD(do_sobel);
    }

Testbench::~Testbench() {}

int Testbench::read_bmp(string infile_name) {
    FILE *fp_s = NULL; // source file handler
    fp_s = fopen(infile_name.c_str(), "r");
    if (fp_s == NULL) {
        printf("fopen %s error\n", infile_name.c_str());
        return -1;
    }

    for (int i = 0; i < 16; i++) {
        fscanf(fp_s, "%d", &A[i]);
    }

    cout << "input A: ";
    for (int i = 0; i < 16; i++) {
        cout << A[i] << " ";
    }
    cout << endl;


    return 0;
}

void Testbench::do_sobel() {
    word data[4];
    word o_data[4];
    unsigned char mask[16];
    //wait(5 * CLOCK_PERIOD, SC_NS);

    for (int i = 0; i < 4; i++) {
        int tmp = i * 4;
        data[i].uc[0] = A[tmp];
        data[i].uc[1] = A[tmp + 1];
        data[i].uc[2] = A[tmp + 2];
        data[i].uc[3] = A[tmp + 3];
    }
    for (int i = 0; i < 16; i++) {
        mask[i] = 0xff;
    }
    initiator.write_to_socket(SOBEL_MM_BASE + SOBEL_FILTER_R_ADDR, mask,
            data[0].uc, 16);

    bool done=false;
    int output_num=0;
    while(!done){
        //std::cout << "read onece" << std::endl;
        initiator.read_from_socket(SOBEL_MM_BASE + SOBEL_FILTER_CHECK_ADDR, mask, data[0].uc, 4);
        output_num = data[0].sint;
        if(output_num>0) done=true;
    }

    initiator.read_from_socket(SOBEL_MM_BASE + SOBEL_FILTER_RESULT_ADDR, mask,
            o_data[0].uc, 16);
    //printf("Testbench: %d %d %d\n", o_data[0].uint, o_data[1].uint, o_data[2].uint);
    //total = data.sint;
    //debug
    //cout << "Now at " << sc_time_stamp() << endl; //print current sc_time
    for (int i = 0 ; i < 4; i++) {
        int tmp = i * 4;
        C[tmp + 0] = o_data[i].uc[0];
        C[tmp + 1] = o_data[i].uc[1];
        C[tmp + 2] = o_data[i].uc[2];
        C[tmp + 3] = o_data[i].uc[3];
    }

    for (int i = 0; i < 16; i++) {
        cout << C[i] << " ";
    }
    cout << endl;

    wait(4610, SC_NS);
    sc_stop();
}
