#include <stdint.h>
#include <stddef.h>
#include <aws/common/common.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_mul_size_saturating_harness() {
    size_t a;
    size_t b;
    size_t r;
    
    int result = aws_mul_size_saturating(a, b, &r);
    
    assert(result == AWS_OP_SUCCESS);
    if (a == (size_t)0 || b == (size_t)0) {
        assert(r == (size_t)0);
    } else if (a > ((size_t)-1) / b) {
        assert(r == ((size_t)-1));
    } else {
        assert(r == a * b);
    }
}
