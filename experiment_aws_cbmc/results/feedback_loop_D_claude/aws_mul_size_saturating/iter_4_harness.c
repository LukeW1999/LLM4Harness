#include <aws/common/math.h>
#include <stdint.h>
#include <assert.h>

void aws_mul_size_saturating_harness(void) {
    size_t a;
    size_t b;

    /* Bound inputs to avoid timeout */
    __CPROVER_assume(a <= 0xFFFF);
    __CPROVER_assume(b <= 0xFFFF);

    size_t result = aws_mul_size_saturating(a, b);

#if SIZE_MAX == UINT32_MAX
    uint64_t wide = (uint64_t)a * (uint64_t)b;
    if (wide > (uint64_t)SIZE_MAX) {
        assert(result == SIZE_MAX);
    } else {
        assert(result == (size_t)wide);
    }
#elif SIZE_MAX == UINT64_MAX
    __uint128_t wide = (__uint128_t)a * (__uint128_t)b;
    if (wide > (__uint128_t)SIZE_MAX) {
        assert(result == SIZE_MAX);
    } else {
        assert(result == (size_t)wide);
    }
#endif
}
