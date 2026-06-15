/*  CBMC harness for aws_is_power_of_two  */

#include <aws/common/math.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* Helper header required by the build system (no allocator usage) */
#include <proof_helpers/make_common_data_structures.h>

void aws_is_power_of_two_harness(void) {
    /* nondeterministic input */
    size_t x;
    __CPROVER_assume(x <= SIZE_MAX);   /* trivially true, keeps CBMC happy */

    /* call the function under test */
    bool result = aws_is_power_of_two(x);

    /* expected logical condition */
    bool expected = (x != 0) && ((x & (x - 1)) == 0);

    /* verify the contract */
    assert(result == expected);

    return 0;
}
