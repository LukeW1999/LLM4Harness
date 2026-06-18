#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_add_size_saturating_harness() {
    size_t a;
    size_t b;
    size_t r;

    __CPROVER_assume(a <= SIZE_MAX);
    __CPROVER_assume(b <= SIZE_MAX);

    int result = aws_add_size_saturating(a, b, &r);
    assert(result == AWS_OP_SUCCESS);

    if (a > SIZE_MAX - b) {
        assert(r == SIZE_MAX);
    } else {
        assert(r == a + b);
    }
}
