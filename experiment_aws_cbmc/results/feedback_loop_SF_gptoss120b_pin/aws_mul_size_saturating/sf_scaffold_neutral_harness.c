#include <aws/common/common.h>
#include <aws/common/math.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_mul_size_saturating_harness(void) {
    /* Symbolic inputs */
    size_t a = (size_t)nondet_uint64_t();
    size_t b = (size_t)nondet_uint64_t();

    /* Precondition assumptions (if any) */
    __CPROVER_assume(a <= SIZE_MAX);
    __CPROVER_assume(b <= SIZE_MAX);

    /* Snapshot of inputs for postcondition checks */
    const size_t a_old = a;
    const size_t b_old = b;

    /* Call the function under verification */
    size_t result = aws_mul_size_saturating(a, b);

    
}
