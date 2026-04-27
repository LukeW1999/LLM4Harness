#include <stdint.h>
#include <stddef.h>
#include <assert.h>
#include <aws/common/common.h>
#include <aws/common/math.h>

/* Harness for aws_add_u64_checked */
void aws_add_u64_checked_harness() {
    uint64_t a = nondet_uint64_t();
    uint64_t b = nondet_uint64_t();
    uint64_t r;

    int result = aws_add_u64_checked(a, b, &r);

    if (result == AWS_OP_SUCCESS) {
        /* On success: no overflow, r == a + b */
        assert(result == 0);
        assert(r == a + b);
        /* No overflow means: b == 0 OR a <= UINT64_MAX - b */
        assert(b == 0 || a <= (UINT64_MAX - b));
    } else {
        /* On failure: overflow detected */
        assert(result == -1);
        /* r is not written on failure path */
        assert(b > 0 && a > (UINT64_MAX - b));
    }
}

/* Harness for aws_add_u32_checked */
void aws_add_u32_checked_harness() {
    uint32_t a = nondet_uint32_t();
    uint32_t b = nondet_uint32_t();
    uint32_t r;

    int result = aws_add_u32_checked(a, b, &r);

    if (result == AWS_OP_SUCCESS) {
        /* On success: no overflow, r == a + b */
        assert(result == 0);
        assert(r == a + b);
        /* No overflow means: b == 0 OR a <= UINT32_MAX - b */
        assert(b == 0 || a <= (UINT32_MAX - b));
    } else {
        /* On failure: overflow detected */
        assert(result == -1);
        assert(b > 0 && a > (UINT32_MAX - b));
    }
}

/* Harness for aws_add_size_checked */
void aws_add_size_checked_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t r;

    int result = aws_add_size_checked(a, b, &r);

    if (result == AWS_OP_SUCCESS) {
        /* On success: no overflow, r == a + b */
        assert(result == 0);
        assert(r == a + b);
        /* No overflow: b == 0 OR a <= SIZE_MAX - b */
        assert(b == 0 || a <= (SIZE_MAX - b));
    } else {
        /* On failure: overflow detected */
        assert(result == -1);
        assert(b > 0 && a > (SIZE_MAX - b));
    }
}
