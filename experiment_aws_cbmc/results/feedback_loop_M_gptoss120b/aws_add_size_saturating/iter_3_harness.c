#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_add_size_saturating_harness(void) {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    size_t result;
    int rc = aws_add_size_saturating(a, b, &result);

    if (a <= SIZE_MAX - b) {
        /* No overflow possible */
        assert(rc == 0);
        assert(result == a + b);
    } else {
        /* Overflow must have occurred */
        assert(rc != 0);
        assert(result == SIZE_MAX);
    }
}
