#include <aws/common/common.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_mul_size_checked_harness() {
    /* nondeterministic inputs */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    /* allocate output pointer */
    size_t *result = (size_t *)malloc(sizeof(size_t));
    __CPROVER_assume(result != NULL);
    *result = nondet_size_t();

    /* save old state */
    size_t old_a = a;
    size_t old_b = b;
    size_t old_result = *result;
    size_t *old_result_ptr = result;

    /* call function under test */
    int rc = aws_mul_size_checked(a, b, result);

    /* postconditions */
    if (rc == AWS_OP_SUCCESS) {
        /* overflow must not have occurred */
        assert(a == 0 || b <= SIZE_MAX / a);
        /* result must be the product */
        assert(*result == a * b);
    } else {
        /* on failure, result must be unchanged */
        assert(*result == old_result);
    }

    /* inputs must remain unchanged */
    assert(a == old_a);
    assert(b == old_b);
    assert(result == old_result_ptr);
    assert(result != NULL);
}
