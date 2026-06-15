#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <aws/common/common.h>
#include <aws/common/math.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_is_power_of_two_harness(void) {
    /* nondeterministic input */
    size_t n = nondet_size_t();

    /* Call the function under test */
    bool is_pow2 = aws_is_power_of_two(n);

    /* Specification: a number is a power of two iff it is non‑zero and
       has exactly one bit set. */
    bool spec = (n != 0) && ((n & (n - 1)) == 0);

    /* Check that the implementation matches the specification */
    assert(is_pow2 == spec);
}
