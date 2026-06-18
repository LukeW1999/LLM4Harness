#include <assert.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_mul_size_saturating_harness(void) {
    size_t a;
    size_t b;
    size_t r = 0;
    
    int result = aws_mul_size_saturating(a, b, &r);
    
    assert(result == 0);
}
