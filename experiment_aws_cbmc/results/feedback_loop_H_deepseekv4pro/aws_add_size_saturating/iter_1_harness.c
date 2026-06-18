#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>

void aws_add_u32_checked_harness() {
    uint32_t a = nondet_uint32_t();
    uint32_t b = nondet_uint32_t();
    uint32_t r;

    int result = aws_add_u32_checked(a, b, &r);

    if (result == AWS_OP_SUCCESS) {
        /* On success, *r must equal the mathematical sum (no overflow) */
        assert((uint64_t)a + (uint64_t)b == r);
        assert(a <= UINT32_MAX - b); /* equivalent no-overflow condition */
    } else {
        /* On failure, overflow must have occurred */
        assert(result == AWS_OP_ERR);
        assert((uint64_t)a + (uint64_t)b > UINT32_MAX);
    }
}
