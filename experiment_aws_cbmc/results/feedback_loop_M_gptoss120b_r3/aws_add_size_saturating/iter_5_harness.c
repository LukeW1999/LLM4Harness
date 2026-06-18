#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <limits.h>
#include <stdbool.h>

size_t nondet_size_t(void);

void aws_add_size_saturating_harness(void) {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    size_t old_a = a;
    size_t old_b = b;

    size_t result;
    size_t *pResult = &result;
    size_t *old_pResult = pResult;

    bool overflow = aws_add_size_saturating(a, b, pResult);

    assert(pResult == old_pResult);
    assert(a == old_a);
    assert(b == old_b);

    if (overflow) {
        assert(*pResult == SIZE_MAX);
        assert(a > SIZE_MAX - b);
    } else {
        assert(*pResult == a + b);
        assert(a <= SIZE_MAX - b);
    }
}
