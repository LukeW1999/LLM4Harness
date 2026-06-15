#include "aws/common/hash_table.h"
#include "proof_helpers/make_common_data_structures.h"
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>

void aws_ptr_eq_harness(void) {
    /* nondeterministic pointer values */
    const void *a = (const void *)nondet_uint64_t();
    const void *b = (const void *)nondet_uint64_t();

    /* Save inputs for later comparison */
    const void *old_a = a;
    const void *old_b = b;

    /* Call the function under test */
    bool result = aws_ptr_eq(a, b);

    /* Postcondition: result reflects pointer equality */
    assert(result == (old_a == old_b));

    /* The inputs must remain unchanged */
    assert(a == old_a);
    assert(b == old_b);
}
