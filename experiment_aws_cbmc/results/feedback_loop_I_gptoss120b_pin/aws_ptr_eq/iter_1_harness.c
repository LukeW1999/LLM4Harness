#include <aws/common/hash_table.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_ptr_eq_harness() {
    /* Non-deterministic pointer values (no dereferencing occurs) */
    const void *a = (void *)nondet_uint64_t();
    const void *b = (void *)nondet_uint64_t();

    /* Call the function under test */
    bool result = aws_ptr_eq(a, b);

    /* Postcondition: result must reflect pointer equality */
    assert(result == (a == b));
}
