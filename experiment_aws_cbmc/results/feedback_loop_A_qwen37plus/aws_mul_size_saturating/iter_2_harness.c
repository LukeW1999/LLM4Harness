#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_mul_size_saturating_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t r;
    
    int result = aws_mul_size_saturating(a, b, &r);
    
    assert(result == AWS_OP_SUCCESS);
    if (a == 0 || b == 0) {
        assert(r == 0);
    } else if (a > SIZE_MAX / b) {
        assert(r == SIZE_MAX);
    } else {
        assert(r == a * b);
    }
}
