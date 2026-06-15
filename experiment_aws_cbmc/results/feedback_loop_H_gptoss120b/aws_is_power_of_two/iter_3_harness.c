#include <assert.h>
#include <stddef.h>
#include "aws/common/common.h"
#include "aws/common/math.h"
#include "proof_helpers/make_common_data_structures.h"

void aws_is_power_of_two_harness() {
    /* 1. Non‑deterministic input */
    size_t n = nondet_size_t();
    __CPROVER_assume(n <= SIZE_MAX);

    /* Save old state */
    size_t old_n = n;

    /* 2. Call the function under test */
    int is_pow = aws_is_power_of_two(n);

    /* 3. Force both true and false outcomes to be explored */
    if (nondet_bool()) {
        /* Assume n is a power of two (and non‑zero) */
        __CPROVER_assume((n != 0) && ((n & (n - 1)) == 0));
        assert(is_pow);
    } else {
        /* Assume n is not a power of two (or zero) */
        __CPROVER_assume(!((n != 0) && ((n & (n - 1)) == 0)));
        assert(!is_pow);
    }

    /* 4. Invariant: input unchanged */
    assert(n == old_n);
}
