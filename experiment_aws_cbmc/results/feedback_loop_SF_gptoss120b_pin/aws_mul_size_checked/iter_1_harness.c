#include <aws/common/common.h>
#include <aws/common/math.h>
#include "proof_helpers/make_common_data_structures.h"
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_mul_size_checked_harness(void) {
    /* Symbolic inputs */
    size_t a = (size_t)__CPROVER_nondet_unsigned_long();
    size_t b = (size_t)__CPROVER_nondet_unsigned_long();

    size_t *r = malloc(sizeof(size_t));
    __CPROVER_assume(r != NULL);

    /* Pre‑condition assumptions (if any) can be added here */
    /* __CPROVER_assume(...); */

    /* Snapshot of input state */
    size_t old_r = *r;

    /* Call the function under verification */
    int result = aws_mul_size_checked(a, b, r);

    /* ASSERT_POSTCONDITIONS_HERE */
    assert(r != NULL);
    if (result == AWS_OP_SUCCESS) {
        assert(*r == a * b);
    } else {
        assert(result == AWS_ERROR_OVERFLOW);
        assert(*r == old_r);
    }
}
