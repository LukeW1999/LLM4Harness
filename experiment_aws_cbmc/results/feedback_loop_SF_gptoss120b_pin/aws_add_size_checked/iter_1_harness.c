#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_add_size_checked_harness(void) {
    /* Symbolic inputs */
    size_t a = (size_t)__CPROVER_nondet_uint64_t();
    size_t b = (size_t)__CPROVER_nondet_uint64_t();

    size_t *r = (size_t *)malloc(sizeof(size_t));
    __CPROVER_assume(r != NULL);
    *r = (size_t)__CPROVER_nondet_uint64_t();

    /* Pre‑call snapshot */
    size_t a_old = a;
    size_t b_old = b;
    size_t r_old = *r;
    size_t *r_ptr_old = r;

    /* Call under verification */
    int result = aws_add_size_checked(a, b, r);

    /* ASSERT_POSTCONDITIONS_HERE */
    /* result must be either success (0) or error (-1) */
    assert(result == 0 || result == -1);

    /* frame conditions: inputs and pointer unchanged */
    assert(a == a_old);
    assert(b == b_old);
    assert(r == r_ptr_old);

    /* post‑condition on the result value */
    if (result == 0) {
        /* on success, *r holds the sum */
        assert(*r == a + b);
    } else {
        /* on overflow error, *r is unchanged */
        assert(*r == r_old);
    }
}
