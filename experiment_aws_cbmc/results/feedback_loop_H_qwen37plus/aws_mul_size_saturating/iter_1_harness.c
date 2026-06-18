#include <aws/common/math.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>

void aws_add_size_checked_harness(void) {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    
    size_t *r = malloc(sizeof(size_t));
    __CPROVER_assume(r != NULL);
    size_t old_r = *r;
    
    int result = aws_add_size_checked(a, b, r);
    
    if (result == AWS_OP_SUCCESS) {
        assert(*r == a + b);
    } else {
        assert(result == AWS_OP_ERR);
        assert(*r == old_r);
    }
    
    free(r);
}
