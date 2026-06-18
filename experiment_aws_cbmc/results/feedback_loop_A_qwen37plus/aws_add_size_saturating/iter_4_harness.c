#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <assert.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_add_size_saturating_harness(void) {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t r;

    int result = aws_add_size_saturating(a, b, &r);

    assert(result == AWS_OP_SUCCESS);
    if (a <= SIZE_MAX - b) {
        assert(r == a + b);
    } else {
        assert(r == SIZE_MAX);
    }
}
