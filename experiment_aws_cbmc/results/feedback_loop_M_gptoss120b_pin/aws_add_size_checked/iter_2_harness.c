#include <aws/common/common.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_add_size_checked_harness(void) {
    /* 1. Non‑deterministic inputs bounded */
    size_t a = nondet_size_t();
    __CPROVER_assume(a <= MAX_BUFFER_SIZE);

    size_t b = nondet_size_t();
    __CPROVER_assume(b <= MAX_BUFFER_SIZE);

    /* 2. Allocate output pointer and bound its initial content */
    size_t r_val = nondet_size_t();
    __CPROVER_assume(r_val <= MAX_BUFFER_SIZE);
    size_t old_r = r_val;
    size_t *r = &r_val;

    /* 3. Call function under test */
    int result = aws_add_size_checked(a, b, r);

    /* 4. Post‑conditions for both success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* On success the sum must be stored without overflow */
        assert(*r == a + b);
    } else {
        /* On failure the output must remain unchanged */
        assert(*r == old_r);
    }

    /* 5. Result must be one of the defined return codes */
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);
}
