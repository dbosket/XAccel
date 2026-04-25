#include <stdint.h>
#include <stdio.h>

extern "C" {

void sha256_block(uint32_t w0, uint32_t w1, uint32_t w2, uint32_t w3,
                  uint32_t w4, uint32_t w5, uint32_t w6, uint32_t w7,
                  uint32_t w8, uint32_t w9, uint32_t w10, uint32_t w11,
                  uint32_t w12, uint32_t w13, uint32_t w14, uint32_t w15,
                  uint32_t *h0, uint32_t *h1, uint32_t *h2, uint32_t *h3,
                  uint32_t *h4, uint32_t *h5, uint32_t *h6, uint32_t *h7);

}

static int check_word(const char *name, uint32_t actual, uint32_t expected)
{
    if (actual != expected) {
        printf("FAIL: %s expected 0x%08x, got 0x%08x\n",
               name, expected, actual);
        return 1;
    }

    printf("PASS: %s = 0x%08x\n", name, actual);
    return 0;
}

int main()
{
    uint32_t h0 = 0;
    uint32_t h1 = 0;
    uint32_t h2 = 0;
    uint32_t h3 = 0;
    uint32_t h4 = 0;
    uint32_t h5 = 0;
    uint32_t h6 = 0;
    uint32_t h7 = 0;

    /*
     * SHA-256 known-answer test for message "abc".
     *
     * Padded block:
     * 61626380 00000000 00000000 00000000
     * 00000000 00000000 00000000 00000000
     * 00000000 00000000 00000000 00000000
     * 00000000 00000000 00000000 00000018
     *
     * Expected digest:
     * ba7816bf 8f01cfea 414140de 5dae2223
     * b00361a3 96177a9c b410ff61 f20015ad
     */

    sha256_block(
        0x61626380, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000,0x00000000,
        0x00000000,0x00000000,0x00000000, 0x00000018,
        &h0, &h1, &h2,&h3,
        &h4, &h5, &h6,&h7);

    int failures = 0;

    failures += check_word("h0", h0, 0xba7816bf);
    failures += check_word("h1", h1, 0x8f01cfea);
    failures += check_word("h2", h2, 0x414140de);
    failures += check_word("h3", h3, 0x5dae2223);
    failures += check_word("h4", h4, 0xb00361a3);
    failures += check_word("h5", h5, 0x96177a9c);
    failures += check_word("h6", h6, 0xb410ff61);
    failures += check_word("h7", h7, 0xf20015ad);

    if (failures != 0) {
        printf("FAIL: sha256_block test failed with %d errors\n", failures);
        return 1;
    }

    printf("PASS: sha256_block test passed\n");
    return 0;
}