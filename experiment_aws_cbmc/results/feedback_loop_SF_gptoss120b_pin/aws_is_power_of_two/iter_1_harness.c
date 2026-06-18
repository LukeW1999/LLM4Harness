#include <aws/common/common.h>
#include <aws/common/math.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_is_power_of_two_harness(void) {
    size_t x = __CPROVER_nondet_size_t();

    /* Precondition: none needed for x (any size_t is valid) */
    __CPROVER_assume(x <= SIZE_MAX);

    /* Snapshot of inputs */
    size_t x_old = x;

    /* Call the function under test */
    bool result = aws_is_power_of_two(x);

    /* ASSERT_POSTCONDITIONS_HERE */
    assert(result == (x_old != 0 && (x_old & (x_old - 1)) == 0));
    assert(x == x_old);
}
