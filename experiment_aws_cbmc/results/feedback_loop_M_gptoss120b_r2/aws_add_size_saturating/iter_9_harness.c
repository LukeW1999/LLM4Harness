#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>

void aws_add_size_saturating_harness(void) {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    size_t old_a = a;
    size_t old_b = b;

    size_t result;
    int ret = aws_add_size_saturating(a, b, &result);

    if (ret == 0) {
        assert(result == old_a + old_b);
    } else {
        assert(ret == -1);
        assert(result == SIZE_MAX);
    }

    assert(a == old_a);
    assert(b == old_b);
}
