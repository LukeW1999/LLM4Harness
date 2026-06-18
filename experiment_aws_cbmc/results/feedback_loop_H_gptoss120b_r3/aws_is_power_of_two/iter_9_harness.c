#include <assert.h>
#include <aws/common/common.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

extern size_t nondet_size_t(void);

void aws_round_up_to_power_of_two_harness(void) {
    /* nondeterministic input */
    size_t n = nondet_size_t();

    /* allocate output variable on the stack */
    size_t result_storage;
    size_t *result = &result_storage;
    *result = nondet_size_t();

    /* save old state */
    size_t old_n = n;
    size_t old_result = *result;

    /* call function under test */
    int rc = aws_round_up_to_power_of_two(n, result);

    /* postconditions */
    if (rc == AWS_OP_SUCCESS) {
        /* result is a power of two and >= n */
        assert(*result >= n);
        assert(aws_is_power_of_two(*result));

        /* special case when n == 0 */
        if (old_n == 0) {
            assert(*result == 1);
        }
    } else {
        /* on failure, *result must be unchanged */
        assert(*result == old_result);
    }

    /* inputs must remain unchanged */
    assert(n == old_n);

    /* result pointer must remain valid */
    assert(result != NULL);
}
