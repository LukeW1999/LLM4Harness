#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

/**
 * Harness for aws_mul_size_checked
 *
 * Assumptions:
 *     - given 2 non-deterministic size_t integers
 *
 * Assertions:
 *     - if aws_mul_size_checked returns AWS_OP_SUCCESS, then *r == a * b
 *     - if aws_mul_size_checked returns AWS_OP_ERR, then the multiplication overflows
 */
void aws_mul_size_checked_harness() {
    if (nondet_bool()) {
        /* 64-bit path */
        uint64_t a = nondet_uint64_t();
        uint64_t b = nondet_uint64_t();
        uint64_t r = nondet_uint64_t();
        int rval = aws_mul_u64_checked(a, b, &r);
        if (rval == AWS_OP_SUCCESS) {
            assert(r == a * b);
        } else {
            assert(a != 0 && b > (UINT64_MAX / a));
        }
    } else {
        /* 32-bit path */
        uint32_t a = nondet_uint32_t();
        uint32_t b = nondet_uint32_t();
        uint32_t r = nondet_uint32_t();
        int rval = aws_mul_u32_checked(a, b, &r);
        if (rval == AWS_OP_SUCCESS) {
            assert(r == a * b);
        } else {
            assert(a != 0 && b > (UINT32_MAX / a));
        }
    }
}
