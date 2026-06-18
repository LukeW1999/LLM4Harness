#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/* Harness for aws_add_size_saturating */
void aws_add_size_saturating_harness(void) {
    /* 1. Non‑deterministic inputs */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    /* 2. Preserve original values */
    size_t old_a = a;
    size_t old_b = b;

    /* 3. Result storage */
    size_t result;
    size_t *pResult = &result;
    size_t *old_pResult = pResult;

    /* 4. Call the function under test */
    bool overflow = aws_add_size_saturating(a, b, pResult);

    /* 5. Post‑conditions */
    /* Pointers must remain unchanged */
    assert(pResult == old_pResult);
    /* Input arguments must remain unchanged */
    assert(a == old_a);
    assert(b == old_b);

    if (overflow) {
        /* On overflow the result is saturated to SIZE_MAX */
        assert(*pResult == SIZE_MAX);
        /* The addition would indeed overflow */
        assert(a > SIZE_MAX - b);
    } else {
        /* No overflow: result equals the sum */
        assert(*pResult == a + b);
        /* The addition does not overflow */
        assert(a <= SIZE_MAX - b);
    }
}
