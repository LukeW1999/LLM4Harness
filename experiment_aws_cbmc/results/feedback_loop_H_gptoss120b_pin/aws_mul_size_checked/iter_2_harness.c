#include <proof_helpers/make_common_data_structures.h>

void aws_mul_size_checked_harness(void) {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t result = 0;

    int ret = aws_mul_size_checked(a, b, &result);

    bool overflow = (a != 0 && b > SIZE_MAX / a);

    if (overflow) {
        /* An overflow should cause a non‑zero return value. */
        assert(ret != 0);
    } else {
        /* No overflow: function must succeed and result must equal the product. */
        assert(ret == 0);
        assert(result == a * b);
    }
}
