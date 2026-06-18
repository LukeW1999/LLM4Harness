#include <aws/common/hash_table.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <proof_helpers/nondet.h>

void aws_ptr_eq_harness(void) {
    /* 1. Non‑deterministic inputs, bounded */
    const void *a;
    const void *b;

    bool a_is_null = nondet_bool();
    __CPROVER_assume(a_is_null == true || a_is_null == false);
    if (a_is_null) {
        a = NULL;
    } else {
        uint8_t *buf_a = malloc(1);
        __CPROVER_assume(buf_a != NULL);
        a = buf_a;
    }

    bool b_is_null = nondet_bool();
    __CPROVER_assume(b_is_null == true || b_is_null == false);
    if (b_is_null) {
        b = NULL;
    } else {
        uint8_t *buf_b = malloc(1);
        __CPROVER_assume(buf_b != NULL);
        b = buf_b;
    }

    /* 2. Save old state */
    const void *old_a = a;
    const void *old_b = b;

    /* 3. Call function under test */
    bool result = aws_ptr_eq(a, b);

    /* 4. Post‑conditions */
    /*   - Result must reflect pointer equality */
    assert(result == (old_a == old_b));

    /*   - Input pointers must remain unchanged */
    assert(a == old_a);
    assert(b == old_b);
}
