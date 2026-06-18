#include <aws/common/math.h>
#include <assert.h>
#include <stdbool.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_mul_size_checked_harness(void) {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    /* Bound inputs to keep verification tractable */
    __CPROVER_assume(a <= 1000);
    __CPROVER_assume(b <= 1000);

    size_t result = 0;
    size_t old_result = result;

    int rc = aws_mul_size_checked(a, b, &result);
    bool overflow = rc != 0;

    if (!overflow) {
        assert(result == a * b);
        if (a != 0) {
            assert(b <= ((size_t)-1) / a);
        }
    } else {
        assert(result == old_result);
        if (a != 0) {
            assert(b > ((size_t)-1) / a);
        } else {
            assert(0);
        }
    }
}
