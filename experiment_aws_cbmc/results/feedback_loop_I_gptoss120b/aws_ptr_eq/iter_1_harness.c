#include <aws/common/hash_table.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdint.h>
#include <stdbool.h>

void aws_ptr_eq_harness(void) {
    /* nondeterministic pointer values */
    const void *a = (void *)nondet_uint64_t();
    const void *b = (void *)nondet_uint64_t();

    /* Save old values for immutability checks */
    const void *old_a = a;
    const void *old_b = b;

    /* Call the function under test */
    bool result = aws_ptr_eq(a, b);

    /* Postcondition: result must reflect pointer equality */
    assert(result == (old_a == old_b));

    /* Inputs must remain unchanged */
    assert(a == old_a);
    assert(b == old_b);
}
