#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <proof_helpers/nondet.h>

/* Prototype of the function under test */
extern bool aws_ptr_eq(const void *a, const void *b);

void aws_ptr_eq_harness(void) {
    /* Non-deterministic pointer values */
    const void *a = (const void *)nondet_uint64_t();
    const void *b = (const void *)nondet_uint64_t();

    /* Call the function */
    bool result = aws_ptr_eq(a, b);

    /* Postcondition: result must reflect pointer equality */
    assert(result == (a == b));
}
