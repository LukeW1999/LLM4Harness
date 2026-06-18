#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <proof_helpers/nondet.h>

/* Declaration of the function under test */
bool aws_ptr_eq(const void *a, const void *b);

void aws_ptr_eq_harness(void) {
    /* Non-deterministic pointer values */
    const void *a = (const void *)nondet_uint64_t();
    const void *b = (const void *)nondet_uint64_t();

    /* Call the function */
    bool result = aws_ptr_eq(a, b);

    /* Postcondition: result must reflect pointer equality */
    assert(result == (a == b));
}
