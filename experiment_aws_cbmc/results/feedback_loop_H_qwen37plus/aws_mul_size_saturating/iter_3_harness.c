#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_mul_size_saturating_harness(void) {
    size_t a = __CPROVER_nondet_size_t();
    size_t b = __CPROVER_nondet_size_t();
    size_t r;
    
    int result = aws_mul_size_saturating(a, b, &r);
    
    assert(result == AWS_OP_SUCCESS);
}
