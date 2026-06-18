#include <assert.h>
#include <stdbool.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

size_t nondet_size_t(void);

void aws_add_size_saturating_harness(void) {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    size_t result = 0;

    bool overflow = aws_add_size_saturating(a, b, &result);

    assert(overflow == (a > ((size_t)-1) - b));

    if (overflow) {
        assert(result == (size_t)-1);
    } else {
        assert(result == a + b);
    }
}
