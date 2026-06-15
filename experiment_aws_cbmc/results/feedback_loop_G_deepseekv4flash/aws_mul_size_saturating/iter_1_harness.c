#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_add_u64_saturating_harness() {
    uint64_t a = nondet_uint64();
    uint64_t b = nondet_uint64();

    uint64_t result = aws_add_u64_saturating(a, b);

    // Postcondition: if overflow, result is UINT64_MAX; otherwise result is a + b
    if (UINT64_MAX - a >= b) {
        // No overflow
        assert(result == a + b);
    } else {
        // Overflow
        assert(result == UINT64_MAX);
    }

    // a and b are unchanged (trivially true since passed by value)
}
