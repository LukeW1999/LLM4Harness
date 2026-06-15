#include <aws/common/math.h>
#include <aws/common/common.h>
#include <stdlib.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_round_up_to_power_of_two_harness(void) {
    /* nondeterministic input */
    size_t n = nondet_size_t();

    /* allocate a writable result pointer and give it a nondeterministic initial value */
    size_t *result = malloc(sizeof(size_t));
    __CPROVER_assume(result != NULL);
    *result = nondet_size_t();

    /* save old state for later comparison */
    size_t old_result = *result;
    size_t old_n = n;

    /* call the function under test */
    int rc = aws_round_up_to_power_of_two(n, result);

    /* n is passed by value, it must remain unchanged */
    assert(n == old_n);

    if (rc == AWS_OP_SUCCESS) {
        /* result must be a power of two */
        assert((*result != 0) && ((*result & (*result - 1)) == 0));

        /* result must be >= original n */
        assert(*result >= old_n);

        /* special case when n == 0 */
        if (old_n == 0) {
            assert(*result == 1);
        }
    } else {
        /* on failure the output pointer must not be modified */
        assert(*result == old_result);
    }

    /* clean up */
    free(result);
}
