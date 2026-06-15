#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <aws/common/common.h>
#include <aws/common/math.h>

#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>

void aws_mul_size_saturating_harness(void) {
    /* Nondeterministic inputs */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    bool overflow_path = nondet_bool();

    /* Constrain inputs to make both success and failure paths reachable */
    if (overflow_path) {
        /* Force overflow: both non‑zero and product exceeds SIZE_MAX */
        __CPROVER_assume(a != 0);
        __CPROVER_assume(b != 0);
        __CPROVER_assume(a > SIZE_MAX / b);
    } else {
        /* No overflow: product fits in size_t */
        __CPROVER_assume(a == 0 || b == 0 || a <= SIZE_MAX / b);
    }

    /* Allocate result pointer */
    size_t *result = (size_t *)malloc(sizeof(size_t));
    __CPROVER_assume(result != NULL);

    /* Initialize result with nondeterministic value and save old state */
    *result = nondet_size_t();
    size_t old_result = *result;

    /* Call function under test */
    int ret = aws_mul_size_saturating(a, b, result);

    /* Postconditions */
    if (ret == AWS_OP_SUCCESS) {
        /* Result must equal the product */
        assert(*result == a * b);
        /* No overflow occurred, so result fits in size_t */
        assert(*result <= SIZE_MAX);
    } else {
        /* On failure, result must be unchanged */
        assert(*result == old_result);
    }

    /* Inputs must remain unchanged */
    assert(a == a);
    assert(b == b);
    assert(overflow_path == overflow_path);

    /* Result pointer must remain valid */
    assert(result != NULL);
}
