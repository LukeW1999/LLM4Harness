#include <aws/common/common.h>
#include <aws/common/math.h>
#include <assert.h>

void aws_add_size_saturating_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t old_a = a;
    size_t old_b = b;

    // Assume that the addition overflows if result is SIZE_MAX
    __CPROVER_assume((a > SIZE_MAX - b) == (aws_add_size_saturating(a, b) == SIZE_MAX));

    size_t result = aws_add_size_saturating(a, b);

    // Step 1: Success path assertions
    if (result != SIZE_MAX) {
        assert(result == old_a + old_b);
    }

    // Step 2: Failure path assertions
    if (result == SIZE_MAX) {
        assert(old_a + old_b < old_a || old_a + old_b < old_b); // Overflow condition
    }

    // Step 3: Frame conditions
    assert(a == old_a);
    assert(b == old_b);
}
