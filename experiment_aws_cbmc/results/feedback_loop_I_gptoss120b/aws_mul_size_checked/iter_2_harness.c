#include <aws/common/math.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>

void aws_mul_size_checked_harness(void) {
    /* nondeterministic operands */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    /* ensure operands are within a reasonable range to allow both overflow and non‑overflow cases */
    __CPROVER_assume(a <= SIZE_MAX);
    __CPROVER_assume(b <= SIZE_MAX);

    /* allocate result storage */
    size_t *result = malloc(sizeof(size_t));
    __CPROVER_assume(result != NULL);

    /* nondeterministic initial value of *result */
    *result = nondet_size_t();

    /* save old state for frame condition checks */
    size_t old_a = a;
    size_t old_b = b;
    size_t old_result_val = *result;
    size_t *old_result_ptr = result;

    /* call the function under verification */
    bool overflow = aws_mul_size_checked(a, b, result);

    /* post‑condition checks */
    if (!overflow) {
        /* no overflow: result must equal the product */
        assert(*result == a * b);
    } else {
        /* overflow: result must be unchanged */
        assert(*result == old_result_val);
    }

    /* frame condition: inputs and pointer identity must be unchanged */
    assert(a == old_a);
    assert(b == old_b);
    assert(result == old_result_ptr);
}
