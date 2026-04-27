#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdint.h>
#include <stddef.h>
#include <assert.h>

void aws_mul_size_saturating_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t r;

    if (nondet_bool()) {
        r = aws_mul_u64_saturating((uint64_t)a, (uint64_t)b);
        if (a > 0 && b > 0 && a > (SIZE_MAX / b)) {
            assert(r == SIZE_MAX);
        } else {
            assert(r == a * b);
        }
    } else {
        r = aws_mul_u32_saturating((uint32_t)a, (uint32_t)b);
        if (a > 0 && b > 0 && a > (SIZE_MAX / b)) {
            assert(r == SIZE_MAX);
        } else {
            assert(r == a * b);
        }
    }
}
