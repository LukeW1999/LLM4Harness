#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdint.h>
#include <stddef.h>
#include <assert.h>

void aws_add_size_saturating_harness() {
    size_t a;
    size_t b;
    size_t r;

    int result = aws_add_size_saturating(a, b, &r);

    if (a > (size_t)-1 - b) {
        assert(r == (size_t)-1);
        assert(result == AWS_OP_SUCCESS);
    } else {
        assert(r == a + b);
        assert(result == AWS_OP_SUCCESS);
    }
}
