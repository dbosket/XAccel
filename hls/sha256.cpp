#include <stdint.h>

// Logical functions for SHA-256
#define ROTR(x, n) (((x) >> (n)) | ((x) << (32 - (n))))
#define SHR(x, n)  ((x) >> (n))

#define CH(x, y, z)  (((x) & (y)) ^ (~(x) & (z)))
#define MAJ(x, y, z) (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))

#define BSIG0(x) (ROTR((x), 2) ^ ROTR((x), 13) ^ ROTR((x), 22))
#define BSIG1(x) (ROTR((x), 6) ^ ROTR((x), 11) ^ ROTR((x), 25))
#define SSIG0(x) (ROTR((x), 7) ^ ROTR((x), 18) ^ SHR((x), 3))
#define SSIG1(x) (ROTR((x), 17) ^ ROTR((x), 19) ^ SHR((x), 10))

// SHA-256 Constants
static const uint32_t k[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
    0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
    0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
    0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
    0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
    0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
    0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
    0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
    0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

extern "C" {

void sha256_block(uint32_t w0, uint32_t w1, uint32_t w2, uint32_t w3, 
                  uint32_t w4, uint32_t w5, uint32_t w6, uint32_t w7, 
                  uint32_t w8, uint32_t w9, uint32_t w10, uint32_t w11,
                  uint32_t w12, uint32_t w13, uint32_t w14, uint32_t w15, 
                  uint32_t *h0, uint32_t *h1, uint32_t *h2, uint32_t *h3,
                  uint32_t *h4, uint32_t *h5, uint32_t *h6, uint32_t *h7)
{
#pragma HLS INTERFACE s_axilite port=w0     bundle=CTRL
#pragma HLS INTERFACE s_axilite port=w1     bundle=CTRL
#pragma HLS INTERFACE s_axilite port=w2     bundle=CTRL
#pragma HLS INTERFACE s_axilite port=w3     bundle=CTRL
#pragma HLS INTERFACE s_axilite port=w4     bundle=CTRL
#pragma HLS INTERFACE s_axilite port=w5     bundle=CTRL
#pragma HLS INTERFACE s_axilite port=w6     bundle=CTRL
#pragma HLS INTERFACE s_axilite port=w7     bundle=CTRL
#pragma HLS INTERFACE s_axilite port=w8     bundle=CTRL
#pragma HLS INTERFACE s_axilite port=w9     bundle=CTRL
#pragma HLS INTERFACE s_axilite port=w10    bundle=CTRL
#pragma HLS INTERFACE s_axilite port=w11    bundle=CTRL
#pragma HLS INTERFACE s_axilite port=w12    bundle=CTRL
#pragma HLS INTERFACE s_axilite port=w13    bundle=CTRL
#pragma HLS INTERFACE s_axilite port=w14    bundle=CTRL
#pragma HLS INTERFACE s_axilite port=w15    bundle=CTRL
#pragma HLS INTERFACE s_axilite port=h0     bundle=CTRL
#pragma HLS INTERFACE s_axilite port=h1     bundle=CTRL
#pragma HLS INTERFACE s_axilite port=h2     bundle=CTRL
#pragma HLS INTERFACE s_axilite port=h3     bundle=CTRL
#pragma HLS INTERFACE s_axilite port=h4     bundle=CTRL
#pragma HLS INTERFACE s_axilite port=h5     bundle=CTRL
#pragma HLS INTERFACE s_axilite port=h6     bundle=CTRL
#pragma HLS INTERFACE s_axilite port=h7     bundle=CTRL
#pragma HLS INTERFACE s_axilite port=return bundle=CTRL

uint32_t w[64];

    w[0]  = w0; w[1]  = w1; w[2]  = w2; w[3]  = w3;
    w[4]  = w4; w[5]  = w5; w[6]  = w6; w[7]  = w7;
    w[8]  = w8; w[9]  = w9; w[10] = w10; w[11] = w11;
    w[12] = w12; w[13] = w13; w[14] = w14; w[15] = w15;

    for (int i = 16; i < 64; i++) 
    {
        w[i] = SSIG1(w[i - 2]) + w[i - 7] + SSIG0(w[i - 15]) + w[i - 16];
    }

    // Init Hash Value
    uint32_t a = 0x6a09e667;
    uint32_t b = 0xbb67ae85;
    uint32_t c = 0x3c6ef372;
    uint32_t d = 0xa54ff53a;
    uint32_t e = 0x510e527f;
    uint32_t f = 0x9b05688c;
    uint32_t g = 0x1f83d9ab;
    uint32_t h = 0x5be0cd19;

    for (int i = 0; i < 64; i++) {
        uint32_t t1 = h + BSIG1(e) + CH(e, f, g) + k[i] + w[i];
        uint32_t t2 = BSIG0(a) + MAJ(a, b, c);

        h = g;
        g = f;
        f = e;
        e = d + t1;
        d = c;
        c = b;
        b = a;
        a = t1 + t2;
    }

    *h0 = 0x6a09e667 + a;
    *h1 = 0xbb67ae85 + b;
    *h2 = 0x3c6ef372 + c;
    *h3 = 0xa54ff53a + d;
    *h4 = 0x510e527f + e;
    *h5 = 0x9b05688c + f;
    *h6 = 0x1f83d9ab + g;
    *h7 = 0x5be0cd19 + h;
}

}