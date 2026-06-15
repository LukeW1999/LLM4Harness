#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_mul_size_saturating_harness(void) {
    /* 1. nondeterministic inputs */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    /* 2. Allocate result storage on the stack */
    size_t result_storage;
    size_t *result = &result_storage;

    /* 3. Call the function under test */
    int rv = aws_mul_size_saturating(a, b, result);

    /* 4. Post‑conditions */
    assert(rv == AWS_OP_SUCCESS || rv == AWS_OP_ERR);

    if (a == 0 || b == 0) {
        /* Zero multiplication case */
        assert(*result == 0);
        assert(rv == AWS_OP_SUCCESS);
    } else if (a > SIZE_MAX / b) {
        /* Overflow case */
        assert(*result == SIZE_MAX);
        assert(rv == AWS_OP_ERR);
    } else {
        /* Normal case */
        assert(*result == a * b);
        assert(rv == AWS_OP_SUCCESS);
    }
}
