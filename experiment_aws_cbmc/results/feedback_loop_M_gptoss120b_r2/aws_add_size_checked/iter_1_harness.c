#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_add_size_checked_harness(void) {
    /* 1. Non‑deterministic inputs, bounded */
    size_t a = nondet_size_t();
    __CPROVER_assume(a <= MAX_BUFFER_SIZE);

    size_t b = nondet_size_t();
    __CPROVER_assume(b <= MAX_BUFFER_SIZE);

    /* 2. Allocate output pointer and remember its original value */
    size_t *r = malloc(sizeof(size_t));
    __CPROVER_assume(r != NULL);
    size_t old_r = *r;   /* nondet initial content */

    /* 3. Save old inputs for immutability checks */
    size_t old_a = a;
    size_t old_b = b;

    /* 4. Call function under test */
    int result = aws_add_size_checked(a, b, r);

    /* 5. Result must be one of the defined outcomes */
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    /* 6. Post‑conditions */
    if (result == AWS_OP_SUCCESS) {
        /* No overflow – the sum is stored in *r */
        assert(*r == a + b);
    } else {
        /* Overflow – *r must remain unchanged */
        assert(*r == old_r);
    }

    /* 7. Inputs are unchanged */
    assert(a == old_a);
    assert(b == old_b);

    /* 8. Clean up */
    free(r);
}
