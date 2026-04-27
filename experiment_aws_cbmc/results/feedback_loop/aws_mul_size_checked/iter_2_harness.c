#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_mul_size_checked_harness() {
    if (nondet_bool()) {
        uint64_t a = (nondet_bool()) ? 0 : UINT64_MAX;
        uint64_t b = nondet_uint64_t();
        uint64_t r = nondet_uint64_t();
        int result = aws_mul_u64_checked(a, b, &r);
        if (result == AWS_OP_SUCCESS) {
            assert(r == a * b);
        } else {
            assert(__CPROVER_overflow_mult(a, b));
        }
    } else {
        uint32_t a = (nondet_bool()) ? 0 : UINT32_MAX;
        uint32_t b = nondet_uint32_t();
        uint32_t r = nondet_uint32_t();
        int result = aws_mul_u32_checked(a, b, &r);
        if (result == AWS_OP_SUCCESS) {
            assert(r == a * b);
        } else {
            assert(__CPROVER_overflow_mult(a, b));
        }
    }
}
