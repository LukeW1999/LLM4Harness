#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <limits.h>

void aws_add_size_checked_harness() {
    /* Non-deterministic inputs */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    /* Output parameter: ensure it has some initial value */
    size_t r = nondet_size_t();
    size_t old_r = r;

    /* Call function under test */
    int result = aws_add_size_checked(a, b, &r);

    /* Postcondition checks */
    if (result == AWS_OP_SUCCESS) {
        /* Must return exactly a + b (no overflow) */
        assert(r == a + b);
    } else {
        /* Must be the error code */
        assert(result == AWS_OP_ERR);
        /* Output parameter must remain unchanged on failure */
        assert(r == old_r);
        /* Overflow must have occurred: a + b > SIZE_MAX */
        assert(a > SIZE_MAX - b);
    }

    /* The return value must always be either success or error */
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);
}
