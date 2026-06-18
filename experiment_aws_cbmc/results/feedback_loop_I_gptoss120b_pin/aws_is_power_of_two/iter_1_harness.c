#include <aws/common/math.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>

void aws_round_up_to_power_of_two_harness() {
    /* nondeterministic input */
    size_t n = nondet_size_t();

    /* allocate output pointer */
    size_t *result = (size_t *)malloc(sizeof(size_t));
    __CPROVER_assume(result != NULL);

    /* save old state */
    size_t old_n = n;
    size_t old_val = *result;

    /* call function under test */
    int ret = aws_round_up_to_power_of_two(n, result);

    /* postconditions */
    if (ret == AWS_OP_SUCCESS) {
        /* result must be a power of two and >= original n */
        assert(aws_is_power_of_two(*result));
        assert(*result >= old_n);
        if (old_n == 0) {
            assert(*result == 1);
        }
    } else {
        /* on failure, result must be unchanged and error code must be AWS_OP_ERR */
        assert(ret == AWS_OP_ERR);
        assert(*result == old_val);
    }

    /* inputs must remain unchanged */
    assert(n == old_n);
    assert(result != NULL);

    free(result);
}
