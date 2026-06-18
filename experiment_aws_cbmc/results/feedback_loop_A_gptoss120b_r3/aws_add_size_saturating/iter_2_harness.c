#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>

void aws_add_size_saturating_harness(void) {
    /* Nondeterministic inputs */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    /* Preserve old values */
    size_t old_a = a;
    size_t old_b = b;

    size_t result_storage = nondet_size_t();
    size_t old_result = result_storage;
    size_t *result = &result_storage;

    /* Call the function under test */
    int rc = aws_add_size_saturating(a, b, result);

    /* Post‑conditions */
    if (rc == 0) {
        /* No overflow: result must be a + b and a must be <= SIZE_MAX - b */
        assert(*result == old_a + old_b);
        assert(old_a <= SIZE_MAX - old_b);
    } else {
        /* Overflow occurred: result must be unchanged and a > SIZE_MAX - b */
        assert(*result == old_result);
        assert(old_a > SIZE_MAX - old_b);
    }
}
