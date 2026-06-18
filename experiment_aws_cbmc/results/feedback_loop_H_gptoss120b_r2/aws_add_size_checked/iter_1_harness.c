#include <aws/common/math.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_add_u64_checked_harness(void) {
    /* nondeterministic inputs */
    uint64_t a = nondet_uint64_t();
    uint64_t b = nondet_uint64_t();

    /* allocate output pointer and give it a nondeterministic initial value */
    uint64_t *r = malloc(sizeof *r);
    __CPROVER_assume(r != NULL);
    uint64_t old_r = nondet_uint64_t();
    *r = old_r;

    /* call the function under test */
    int result = aws_add_u64_checked(a, b, r);

    /* result must be one of the defined return codes */
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (result == AWS_OP_SUCCESS) {
        /* on success the sum is stored in *r */
        assert(*r == a + b);
    } else {
        /* on overflow error the output pointer must remain unchanged */
        assert(*r == old_r);
    }

    /* inputs a and b are not modified (they are passed by value) */
    /* no additional structure to validate */

    free(r);
}
