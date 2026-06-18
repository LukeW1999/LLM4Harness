#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_add_size_checked_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t *r = malloc(sizeof(*r));
    __CPROVER_assume(r != NULL);
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(r, sizeof(*r)));

    size_t old_r = *r;

    int result = aws_add_size_checked(a, b, r);

    /* Postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* Success: *r must be the exact sum, and no overflow occurred */
        assert(*r == a + b);
        assert(a <= SIZE_MAX - b);
    } else {
        /* Failure: overflow must have occurred */
        assert(result == AWS_OP_ERR);
        assert(a > SIZE_MAX - b);
        /* *r must remain unchanged */
        assert(*r == old_r);
    }

    /* a and b are not modified */
    (void)a;
    (void)b;

    free(r);
}
