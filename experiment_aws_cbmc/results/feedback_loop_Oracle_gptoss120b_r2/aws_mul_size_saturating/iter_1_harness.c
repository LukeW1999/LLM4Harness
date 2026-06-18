#include <aws/common/math.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_mul_size_saturating_harness(void) {
    /* nondeterministic inputs */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    /* call the function under test */
    size_t result = aws_mul_size_saturating(a, b);

    /* overflow detection according to the contract */
    bool overflow = (b != 0 && a > SIZE_MAX / b);

    /* post‑condition: result is either the exact product or SIZE_MAX on overflow */
    if (!overflow) {
        assert(result == a * b);
    } else {
        assert(result == SIZE_MAX);
    }

    /* frame condition: the function must not modify any memory outside its
       own locals and parameters.  Since the function has no side‑effects,
       we simply return. */
    return 0;
}
