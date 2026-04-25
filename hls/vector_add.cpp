#include <cstdint>
#include <stdint.h>

extern "C"{

void vector_add(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3,
                uint32_t b0, uint32_t b1, uint32_t b2, uint32_t b3,
                uint32_t *c0, uint32_t *c1, uint32_t *c2, uint32_t *c3)
{
    #pragma HLS INTERFACE s_axilite port=a0          bundle=CTRL
    #pragma HLS INTERFACE s_axilite port=a1          bundle=CTRL
    #pragma HLS INTERFACE s_axilite port=a2          bundle=CTRL
    #pragma HLS INTERFACE s_axilite port=a3          bundle=CTRL

    #pragma HLS INTERFACE s_axilite port=b0          bundle=CTRL
    #pragma HLS INTERFACE s_axilite port=b1          bundle=CTRL
    #pragma HLS INTERFACE s_axilite port=b2          bundle=CTRL
    #pragma HLS INTERFACE s_axilite port=b3          bundle=CTRL

    #pragma HLS INTERFACE s_axilite port=c0          bundle=CTRL
    #pragma HLS INTERFACE s_axilite port=c1          bundle=CTRL
    #pragma HLS INTERFACE s_axilite port=c2          bundle=CTRL
    #pragma HLS INTERFACE s_axilite port=c3          bundle=CTRL

    #pragma HLS INTERFACE s_axilite port=return     bundle=CTRL
        
        *c0 = a0 + b0;
        *c1 = a1 + b1;
        *c2 = a2 + b2;
        *c3 = a3 + b3;
}
}

