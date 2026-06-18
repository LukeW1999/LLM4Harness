#include <aws/common/hash_table.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

void aws_ptr_eq_harness(void) {
    /* 1. Declare nondeterministic inputs */
    const void *a;
    const void *b;

    /* Allocate a non‑NULL pointer for a */
    a = malloc(1);
    __CPROVER_assume(a != NULL);

    /* Nondeterministically decide whether b is the same pointer as a */
    bool same = nondet_bool();
    if (same) {
        b = a;
    } else {
        b = malloc(1);
        __CPROVER_assume(b != NULL);
    }

    /* 2. Save old state of inputs */
    const void *old_a = a;
    const void *old_b = b;

    /* 3. Call the function under test */
    bool result = aws_ptr_eq(a, b);

    /* 4. Assert postconditions */
    /* The result must reflect pointer equality */
    assert(result == (old_a == old_b));

    /* 5. Assert that inputs are unchanged */
    assert(a == old_a);
    assert(b == old_b);
}
