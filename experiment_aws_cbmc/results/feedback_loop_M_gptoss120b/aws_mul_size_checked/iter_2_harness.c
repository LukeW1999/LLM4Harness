#include <aws/common/common.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_mul_size_checked_harness(void) {
    /* 1. Nondeterministic inputs */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    bool result_is_null = nondet_bool();

    size_t result_storage;
    size_t *result_ptr = result_is_null ? NULL : &result_storage;

    /* 2. Call function under test */
    bool overflow = aws_mul_size_checked(a, b, result_ptr);

    /* 3. Post‑condition checks */
    if (result_is_null) {
        /* When result pointer is NULL the function must indicate overflow */
        assert(overflow);
    } else {
        if (overflow) {
            /* Overflow must be genuine */
            assert(a != 0 && b != 0 && a > SIZE_MAX / b);
        } else {
            /* No overflow: result must equal the product */
            assert(result_storage == a * b);
        }
    }
}
