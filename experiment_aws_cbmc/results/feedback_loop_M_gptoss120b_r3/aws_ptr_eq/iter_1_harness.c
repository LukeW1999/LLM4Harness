#include <aws/common/hash_table.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

void aws_ptr_eq_harness() {
    /* Non-deterministic pointers */
    const void *a = (const void *)nondet_uint64_t();
    const void *b = (const void *)nondet_uint64_t();

    /* Call the function under test */
    bool result = aws_ptr_eq(a, b);

    /* Postcondition: result must reflect pointer equality */
    assert(result == (a == b));
}
