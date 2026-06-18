#include <aws/common/math.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_add_size_checked_harness(void) {
    /* 1. Non‑deterministic inputs, bounded */
    size_t a = nondet_size_t();
    __CPROVER_assume(a <= MAX_BUFFER_SIZE);
    size_t b = nondet_size_t();
    __CPROVER_assume(b <= MAX_BUFFER_SIZE);

    /* 2. Output variable */
    size_t r = nondet_size_t();
    size_t *r_ptr = &r;

    /* 3. Save old state */
    size_t old_a = a;
    size_t old_b = b;
    size_t old_r_val = r;
    size_t *old_r_ptr = r_ptr;

    /* 4. Call function under test */
    int result = aws_add_size_checked(a, b, r_ptr);

    /* 5. Post‑conditions for both return paths */
    if (result == AWS_OP_SUCCESS) {
        /* Success: no overflow, result stored correctly */
        assert(*r_ptr == old_a + old_b);
    } else {
        /* Failure: overflow detected, output unchanged */
        assert(result == AWS_OP_ERR);
        assert(r_ptr == old_r_ptr);
        assert(*r_ptr == old_r_val);
    }

    /* 6. Unchanged inputs */
    assert(a == old_a);
    assert(b == old_b);
    assert(r_ptr == old_r_ptr);
}
