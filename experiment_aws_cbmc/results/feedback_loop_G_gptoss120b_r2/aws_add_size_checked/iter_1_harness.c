#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_add_size_checked_harness(void) {
    /* nondeterministic inputs */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    /* allocate output pointer */
    size_t *r = malloc(sizeof(size_t));
    __CPROVER_assume(r != NULL);

    /* save old state of *r */
    size_t old_r = *r;

    /* call function under test */
    int result = aws_add_size_checked(a, b, r);

    /* result must be one of the defined outcomes */
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (result == AWS_OP_SUCCESS) {
        /* success: no overflow occurred */
        assert(b == 0 || a <= (SIZE_MAX - b));
        /* output pointer receives the sum */
        assert(*r == a + b);
    } else {
        /* failure: overflow was detected */
        assert(b > 0);
        assert(a > (SIZE_MAX - b));
        /* output pointer must remain unchanged */
        assert(*r == old_r);
    }

    /* clean up */
    free(r);
}
