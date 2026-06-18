#include <aws/common/math.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_round_up_to_power_of_two_harness(void) {
    /* nondeterministic input */
    size_t n = nondet_size_t();

    /* allocate output pointer */
    size_t *result = malloc(sizeof(size_t));
    __CPROVER_assume(result != NULL);

    /* save old state */
    size_t old_n = n;
    size_t old_result = *result;

    /* call function under test */
    int rc = aws_round_up_to_power_of_two(n, result);

    /* postconditions */
    if (rc == AWS_OP_SUCCESS) {
        /* result must be a power of two and >= n */
        assert(*result >= n);
        assert(aws_is_power_of_two(*result));
        /* overflow cannot have occurred, result fits in size_t (implicitly true) */
    } else {
        /* on error, output must be unchanged */
        assert(*result == old_result);
    }

    /* inputs must remain unchanged */
    assert(n == old_n);
    assert(result != NULL); /* pointer itself must stay valid */

    /* no other side‑effects expected */
}
