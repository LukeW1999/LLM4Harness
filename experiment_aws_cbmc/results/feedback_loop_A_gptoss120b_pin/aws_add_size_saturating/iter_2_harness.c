#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_add_size_saturating_harness(void) {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    size_t result;
    size_t *presult = &result;

    size_t old_a = a;
    size_t old_b = b;

    int ret = aws_add_size_saturating(a, b, presult);

    if (ret == 0) {
        /* Success: no overflow occurred */
        assert(*presult == old_a + old_b);
        assert(SIZE_MAX - old_a >= old_b);
    } else {
        /* Overflow: result should be saturated to SIZE_MAX */
        assert(*presult == SIZE_MAX);
        assert(SIZE_MAX - old_a < old_b);
    }
}
