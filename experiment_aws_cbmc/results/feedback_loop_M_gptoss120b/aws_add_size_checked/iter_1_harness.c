#include <aws/common/math.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>

/* MAX_BUFFER_SIZE is provided by the build system */
void aws_add_size_checked_harness(void) {
    /* 1. Declare and bound scalar inputs */
    size_t a = nondet_size_t();
    __CPROVER_assume(a <= MAX_BUFFER_SIZE);

    size_t b = nondet_size_t();
    __CPROVER_assume(b <= MAX_BUFFER_SIZE);

    /* 2. Declare output pointer and bound its initial value */
    size_t r = nondet_size_t();
    __CPROVER_assume(r <= MAX_BUFFER_SIZE);

    /* 3. Save old state */
    size_t old_a = a;
    size_t old_b = b;
    size_t old_r = r;

    /* 4. Call function under test */
    int result = aws_add_size_checked(a, b, &r);

    /* 5. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* Success: sum fits, result stored in *r */
        assert(r == a + b);
    } else {
        /* Failure: overflow detected, *r unchanged */
        assert(result == AWS_OP_ERR);
        assert(r == old_r);
    }

    /* 6. Inputs must remain unchanged */
    assert(a == old_a);
    assert(b == old_b);
}
