#include <stdint.h>
#include <stdio.h>

extern "C" {

void vector_add( uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3,
                 uint32_t b0, uint32_t b1, uint32_t b2, uint32_t b3,
                 uint32_t *c0, uint32_t *c1, uint32_t *c2, uint32_t *c3);
}

static int check_result(const char *name, uint32_t actual, uint32_t expected)
{
    if (actual != expected) {
        printf("FAIL: %s expected %u, got %u\n", name, expected, actual);
        return 1;
    }

    printf("PASS: %s = %u\n", name, actual);
    return 0;
}

int main()
{
    uint32_t c0 = 0;
    uint32_t c1 = 0;
    uint32_t c2 = 0;
    uint32_t c3 = 0;

    vector_add(
        10, 20, 30, 40,
        1,  2,  3,  4,
        &c0, &c1, &c2, &c3);

    int failures = 0;

    failures += check_result("c0", c0, 11);
    failures += check_result("c1", c1, 22);
    failures += check_result("c2", c2, 33);
    failures += check_result("c3", c3, 44);

    if (failures != 0) {
        printf("FAIL: vector_add test failed with %d errors\n", failures);
        return 1;
    }

    printf("PASS: vector_add test passed\n");
    return 0;
}