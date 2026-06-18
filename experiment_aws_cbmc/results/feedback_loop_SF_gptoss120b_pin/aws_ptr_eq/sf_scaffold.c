#include <aws/common/hash_table.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* Nondeterministic pointer generator for CBMC */
void *nondet_void_ptr(void);

void aws_ptr_eq_harness(void) {
    /* Symbolic inputs */
    const void *a = nondet_void_ptr();
    const void *b = nondet_void_ptr();

    /* Preconditions (if any) can be added here */
    __CPROVER_assume(a == NULL || __CPROVER_is_fresh(a, sizeof(void *)));
    __CPROVER_assume(b == NULL || __CPROVER_is_fresh(b, sizeof(void *)));

    /* Snapshot of inputs for post‑condition checks */
    const void *a_old = a;
    const void *b_old = b;

    /* Call the function under verification */
    bool result = aws_ptr_eq(a, b);

    /* ASSERT_POSTCONDITIONS_HERE */
}
