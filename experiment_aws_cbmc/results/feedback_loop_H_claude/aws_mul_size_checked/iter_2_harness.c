#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t r;

    int result = aws_mul_size_checked(a, b, &r);

    bool overflow = (a != 0) && (b > SIZE_MAX / a);

    if (result == AWS_OP_SUCCESS) {
        assert(result == 0);
        assert(r == a * b);
        assert(!overflow);
    } else {
        assert(result == -1);
        assert(overflow);
    }
}
