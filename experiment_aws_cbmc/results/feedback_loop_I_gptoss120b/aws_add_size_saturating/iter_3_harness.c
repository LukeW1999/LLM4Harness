#include <aws/common/math.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_add_size_saturating_harness(void) {
    /* 1. Non‑deterministic inputs */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    /* 2. Allocate result storage */
    size_t *out = malloc(sizeof(size_t));
    __CPROVER_assume(out != NULL);
    size_t *out_orig = out;

    /* Initialize *out with a nondet value to check frame condition */
    *out = nondet_size_t();

    /* 3. Call the function under test */
    int ret = aws_add_size_saturating(a, b, out);

    /* 4. Post‑conditions */

    /* validity_predicate: return value must be success or overflow */
    assert(ret == AWS_OP_SUCCESS || ret == AWS_ERROR_OVERFLOW);

    /* length_invariant: result value respects saturation semantics */
    if (ret == AWS_OP_SUCCESS) {
        assert(*out == a + b);
    } else {
        assert(*out == SIZE_MAX);
    }

    /* frame_condition: the pointer itself must remain unchanged and non‑null */
    assert(out == out_orig);
    assert(out != NULL);

    /* 5. Clean up */
    free(out);
}
