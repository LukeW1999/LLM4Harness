#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdbool.h>
#include <limits.h>

extern size_t nondet_size_t(void);

void aws_add_size_saturating_harness(void) {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    size_t old_a = a;
    size_t old_b = b;

    size_t out;
    bool overflow = aws_add_size_saturating(a, b, &out);

    if (overflow) {
        assert(out == SIZE_MAX);
        assert(SIZE_MAX - a < b);
    } else {
        assert(out == a + b);
        assert(SIZE_MAX - a >= b);
    }

    assert(a == old_a);
    assert(b == old_b);
}
