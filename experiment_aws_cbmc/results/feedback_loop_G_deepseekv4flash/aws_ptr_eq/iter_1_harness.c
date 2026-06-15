#include <aws/common/hash_table.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/proof_allocators.h>

void aws_ptr_eq_harness() {
    /* Non-deterministic inputs */
    const void *a = nondet_voidp();
    const void *b = nondet_voidp();

    /* Call the function */
    bool result = aws_ptr_eq(a, b);

    /* Postcondition: result is true iff a and b point to the same address */
    assert(result == (a == b));
}
