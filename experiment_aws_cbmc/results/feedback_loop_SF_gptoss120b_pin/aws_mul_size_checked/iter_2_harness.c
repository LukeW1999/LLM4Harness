#include <aws/common/common.h>
#include <aws/common/math.h>
#include <aws/common/error.h>
#include "proof_helpers/make_common_data_structures.h"
#include <assert.h>
#include <stdint.h>
#include <stdbool.h>

void aws_mul_size_checked_harness(void) {
    /* Symbolic inputs */
    size_t a = (size_t)__CPROVER_nondet_unsigned_long();
    size_t b = (size_t)__CPROVER_nondet_unsigned_long();

    /* Stack‑allocated result storage */
    size_t r;
    size_t *pr = &r;

    /* Snapshot of input state */
    size_t old_r = r;

    /* Call the function under verification */
    int result = aws_mul_size_checked(a, b, pr);

    /* Post‑condition checks */
    assert(pr != NULL);
    if (result == AWS_OP_SUCCESS) {
        assert(*pr == a * b);
    } else {
        assert(result == AWS_ERROR_OVERFLOW);
        assert(*pr == old_r);
    }
}
