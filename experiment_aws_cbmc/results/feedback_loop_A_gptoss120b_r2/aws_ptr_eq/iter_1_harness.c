#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* Declaration of the function under verification */
bool aws_ptr_eq(const void *a, const void *b);

void aws_ptr_eq_harness(void) {
    /* 1. Declare nondeterministic inputs */
    const void *a = nondet_uint64_t() ? (void *)nondet_uint64_t() : NULL;
    const void *b = nondet_uint64_t() ? (void *)nondet_uint64_t() : NULL;

    /* 2. Save old state */
    const void *old_a = a;
    const void *old_b = b;

    /* 3. Call the function under test */
    bool result = aws_ptr_eq(a, b);

    /* 4. Assert postconditions */
    /* The function must return true exactly when the two pointers are equal */
    assert(result == (a == b));

    /* 5. Assert that inputs are unchanged */
    assert(a == old_a);
    assert(b == old_b);

    return 0;
}
