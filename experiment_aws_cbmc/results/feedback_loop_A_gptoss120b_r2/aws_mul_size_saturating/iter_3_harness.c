#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <limits.h>

void aws_mul_size_saturating_harness(void) {
    /* nondeterministic inputs */
    size_t a = (size_t)nondet_uint64_t();
    size_t b = (size_t)nondet_uint64_t();

    /* preserve original values */
    size_t old_a = a;
    size_t old_b = b;

    /* result buffer */
    size_t result = (size_t)nondet_uint64_t();
    size_t *p_result = &result;

    /* call function under test */
    int ret = aws_mul_size_saturating(a, b, p_result);

    /* inputs must remain unchanged */
    assert(a == old_a);
    assert(b == old_b);

    if (ret == 0) {
        /* no overflow: result must equal a * b */
        assert(*p_result == a * b);
    } else {
        /* overflow: result must be saturated to SIZE_MAX */
        assert(*p_result == SIZE_MAX);
    }
}
