#include <aws/common/math.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_add_size_checked_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t r;
    
    int result = aws_add_size_checked(a, b, &r);
    
    if (result == AWS_OP_SUCCESS) {
        assert(!__CPROVER_overflow_plus(a, b));
        assert(r == a + b);
    } else {
        assert(result == AWS_OP_ERR);
        assert(__CPROVER_overflow_plus(a, b));
    }
}
