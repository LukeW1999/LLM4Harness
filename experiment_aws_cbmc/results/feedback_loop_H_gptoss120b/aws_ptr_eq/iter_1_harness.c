#include <aws/common/hash_table.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_ptr_eq_harness(void) {
    /* 1. Declare nondeterministic inputs */
    const void *a = (const void *)nondet_uint64_t();
    const void *b = (const void *)nondet_uint64_t();

    /* 2. Save old state of inputs */
    const void *old_a = a;
    const void *old_b = b;

    /* 3. Call function under test */
    bool result = aws_ptr_eq(a, b);

    /* 4. Postconditions */
    /* The function must return true iff the two pointers are equal */
    assert(result == (a == b));

    /* 5. Unchanged inputs */
    assert(a == old_a);
    assert(b == old_b);

    /* 6. Result is a proper boolean value */
    assert(result == 0 || result == 1);
}
