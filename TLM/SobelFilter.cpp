#include <cmath>
#include <iomanip>

#include "SobelFilter.h"

SobelFilter::SobelFilter(sc_module_name n)
    : sc_module(n), t_skt("t_skt"), base_offset(0) {
        SC_THREAD(do_filter);

        t_skt.register_b_transport(this, &SobelFilter::blocking_transport);
    }

SobelFilter::~SobelFilter() = default;

void SobelFilter::do_filter() {
    int A[16], C[16];
    while (true) {
        for (int i = 0; i < 16; i++) {
            A[i] = i_array.read();
        }
        // caculate
        for (int w = 1; w < 16; w = w * 2) {
            // make pair
            for (int first = 0; first < 16; first += 2 * w) {
                int second = first + w;
                int end = second + w - 1;
                int l1 = first, l2 = second;
                int index = first;
                // merge
                for (int i = 0; i < 2 * w; i++) {
                    if (l2 <= end && (A[l1] > A[l2] || l1 >= second)) {
                        C[index++] = A[l2++];
                    } else {
                        C[index++] = A[l1++];
                    }
                }
            }

            for (int i = 0; i < 16; i++) {
                A[i] = C[i];
            }
        }

        for (int i = 0; i < 16; i++) {
            o_array.write(A[i]);
        }
    }
}

void SobelFilter::blocking_transport(tlm::tlm_generic_payload &payload,
        sc_core::sc_time &delay) {
    sc_dt::uint64 addr = payload.get_address();
    addr -= base_offset;
    unsigned char *mask_ptr = payload.get_byte_enable_ptr();
    unsigned char *data_ptr = payload.get_data_ptr();
    //word buffer;
    word o_buffer[4];
    switch (payload.get_command()) {
        case tlm::TLM_READ_COMMAND:
            //delay = sc_core::sc_time(0, SC_NS);
            switch (addr) {
                case SOBEL_FILTER_RESULT_ADDR:
                    for (int i = 0; i < 4; i++) {
                        o_buffer[i].uc[0] = o_array.read();
                        o_buffer[i].uc[1] = o_array.read();
                        o_buffer[i].uc[2] = o_array.read();
                        o_buffer[i].uc[3] = o_array.read();
                    }
                    delay = sc_core::sc_time(10 * 1, SC_NS);
                    break;
                case SOBEL_FILTER_CHECK_ADDR:
                    if (o_array.num_available())
                        o_buffer[0].uint = 1;
                    else 
                        o_buffer[0].uint = 0;
                    break;
                default:
                    std::cerr << "Error! SobelFilter::blocking_transport: address 0x"
                        << std::setfill('0') << std::setw(8) << std::hex << addr
                        << std::dec << " is not valid" << std::endl;
            }
            data_ptr[0] = o_buffer[0].uc[0];
            data_ptr[1] = o_buffer[0].uc[1];
            data_ptr[2] = o_buffer[0].uc[2];
            data_ptr[3] = o_buffer[0].uc[3];
            
            data_ptr[4] = o_buffer[1].uc[0];
            data_ptr[5] = o_buffer[1].uc[1];
            data_ptr[6] = o_buffer[1].uc[2];
            data_ptr[7] = o_buffer[1].uc[3];
            
            data_ptr[8]  = o_buffer[2].uc[0];
            data_ptr[9]  = o_buffer[2].uc[1];
            data_ptr[10] = o_buffer[2].uc[2];
            data_ptr[11] = o_buffer[2].uc[3];

            data_ptr[12] = o_buffer[3].uc[0];
            data_ptr[13] = o_buffer[3].uc[1];
            data_ptr[14] = o_buffer[3].uc[2];
            data_ptr[15] = o_buffer[3].uc[3];
            break;
        case tlm::TLM_WRITE_COMMAND:
            switch (addr) {
                case SOBEL_FILTER_R_ADDR:
                    for (int i = 0; i < 16; i++) {
                        mask_ptr[i] = 0xff;
                        i_array.write(data_ptr[i]);
                    }
                    delay = sc_core::sc_time(10 * 1, SC_NS);
                    break;
                default:
                    std::cerr << "Error! SobelFilter::blocking_transport: address 0x"
                        << std::setfill('0') << std::setw(8) << std::hex << addr
                        << std::dec << " is not valid" << std::endl;
            }
            break;
        case tlm::TLM_IGNORE_COMMAND:
            payload.set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
            return;
        default:
            payload.set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
            return;
    }
    payload.set_response_status(tlm::TLM_OK_RESPONSE); // Always OK
}
