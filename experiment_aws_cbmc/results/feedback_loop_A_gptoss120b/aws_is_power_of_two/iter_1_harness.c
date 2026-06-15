#include <assert.h>
#include <stddef.h>
#include <aws/common/common.h>
#include <aws/common/math.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_round_up_to_power_of_two_harness() {
    /* nondeterministic input */
    size_t n = nondet_size_t();

    /* bound n to the range of size_t (optional, can be omitted) */
    __CPROVER_assume(n <= SIZE_MAX);

    /* allocate a result location */
    size_t result_storage;
    size_t *result = &result_storage;

    /* save old value of *result for the failure case */
    size_t old_result = *result;

    /* call the function under test */
    int rc = aws_round_up_to_power_of_two(n, result);

    /* postconditions */
    if (rc == AWS_OP_SUCCESS) {
        /* *result must be a power of two */
        assert((*result != 0) && ((*result & (*result - 1)) == 0));

        /* *result must be >= original n */
        assert(*result >= n);

        /* special case when n == 0 */
        if (n == 0) {
            assert(*result == 1);
        }
    } else {
        /* on failure, *result must be unchanged */
        assert(*result == old_result);
    }

    /* n is an input value and must remain unchanged (trivially true) */
    /* result pointer itself must not change */
    assert(result == &result_storage);
}
