#include <aws/common/hash_table.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

void aws_ptr_eq_harness(void) {
    /* nondeterministic pointers (may be NULL or any address) */
    const void *a = (void *)nondet_uint64_t();
    const void *b = (void *)nondet_uint64_t();

    /* save old values for immutability checks */
    const void *old_a = a;
    const void *old_b = b;

    /* call the function under test */
    bool result = aws_ptr_eq(a, b);

    /* postcondition: result reflects pointer equality */
    assert(result == (old_a == old_b));

    /* inputs must remain unchanged */
    assert(a == old_a);
    assert(b == old_b);
}
