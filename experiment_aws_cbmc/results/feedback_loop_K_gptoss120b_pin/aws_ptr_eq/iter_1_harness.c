/*=== Contract for aws_ptr_eq ===
Preconditions:
  - No specific preconditions; pointers a and b may be any value (including NULL).
  - No assumptions about the validity of the memory they point to, as the function does not dereference them.

Postconditions (validity):
  - The function returns a bool value.
  - Return value is true iff the two input pointers are equal (a == b).

Postconditions (frame):
  - The function does not modify any memory locations; it is side‑effect free.
===*/

#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* Declaration of the function under test */
bool aws_ptr_eq(const void *a, const void *b);

/* Helper to obtain a nondeterministic pointer value */
static void *nondet_ptr(void) {
    uintptr_t v = (uintptr_t)malloc(sizeof(uintptr_t)); /* allocate dummy memory to get a valid address space */
    __CPROVER_assume(v != 0);
    free(v);
    return (void *)v;
}

void aws_ptr_eq_harness(void) {
    /* Generate nondeterministic pointers */
    const void *a = (const void *)nondet_ptr();
    const void *b = (const void *)nondet_ptr();

    /* Call the function under verification */
    bool result = aws_ptr_eq(a, b);

    /* Verify postconditions */
    assert(result == (a == b));

    return 0;
}
