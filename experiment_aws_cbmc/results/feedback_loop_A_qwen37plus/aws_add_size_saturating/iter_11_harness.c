#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <assert.h>
#include <limits.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_add_size_saturating_harness(void) {
    size_t a;
    size_t b;
    size_t r;

    int result = aws_add_size_saturating(a, b, &r);

    if (a <= SIZE_MAX - b) {
        assert(result == AWS_OP_SUCCESS);
        assert(r == a + b);
    } else {
        assert(result == AWS_OP_ERR);
        assert(r == SIZE_MAX);
    }
}
