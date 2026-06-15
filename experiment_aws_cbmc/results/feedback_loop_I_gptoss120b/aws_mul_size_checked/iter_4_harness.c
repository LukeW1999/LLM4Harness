#include <aws/common/math.h>
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_mul_size_checked_harness(void) {
    /* nondeterministic operands */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    /* limit the range to keep CBMC tractable */
    __CPROVER_assume(a <= 1000);
    __CPROVER_assume(b <= 1000);

    /* nondeterministic initial value of result */
    size_t result_storage = nondet_size_t();
    size_t *result = &result_storage;

    /* save old state for frame condition checks */
    size_t old_a = a;
    size_t old_b = b;
    size_t old_result_val = result_storage;

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
    assert(result == &result_storage);
}
