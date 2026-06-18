#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_add_u64_checked_harness() {
    uint64_t a = nondet_uint64_t();
    uint64_t b = nondet_uint64_t();
    uint64_t r;

    int result = aws_add_u64_checked(a, b, &r);

    if (result == AWS_OP_SUCCESS) {
        /* On success: r == a + b, no overflow occurred */
        assert(r == a + b);
        /* Overflow did not occur means a + b <= UINT64_MAX */
        assert(b == 0 || a <= (UINT64_MAX - b));
    } else {
        /* On failure: overflow was detected */
        assert(result == AWS_OP_ERR);
        assert(b > 0 && a > (UINT64_MAX - b));
    }
}

void aws_add_u32_checked_harness() {
    uint32_t a = nondet_uint32_t();
    uint32_t b = nondet_uint32_t();
    uint32_t r;

    int result = aws_add_u32_checked(a, b, &r);

    if (result == AWS_OP_SUCCESS) {
        /* On success: r == a + b, no overflow occurred */
        assert(r == a + b);
        /* Overflow did not occur means a + b <= UINT32_MAX */
        assert(b == 0 || a <= (UINT32_MAX - b));
    } else {
        /* On failure: overflow was detected */
        assert(result == AWS_OP_ERR);
        assert(b > 0 && a > (UINT32_MAX - b));
    }
}

void aws_add_size_checked_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t r;

    int result = aws_add_size_checked(a, b, &r);

    if (result == AWS_OP_SUCCESS) {
        /* On success: r == a + b, no overflow occurred */
        assert(r == a + b);
        /* Overflow did not occur means a + b <= SIZE_MAX */
        assert(b == 0 || a <= (SIZE_MAX - b));
    } else {
        /* On failure: overflow was detected */
        assert(result == AWS_OP_ERR);
        assert(b > 0 && a > (SIZE_MAX - b));
    }
}
