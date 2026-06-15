#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "aws/common/byte_buf.h"

/* Assume a reasonable bound for the lengths (provided by the build system) */
#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 1024
#endif

void aws_array_eq_harness(void) {
    /* 1. Declare nondeterministic inputs */
    const void *array_a;
    const void *array_b;
    size_t len_a = nondet_size_t();
    size_t len_b = nondet_size_t();

    /* 2. Bound the lengths to keep the state space finite */
    __CPROVER_assume(len_a <= MAX_BUFFER_SIZE);
    __CPROVER_assume(len_b <= MAX_BUFFER_SIZE);

    /* 3. Allocate memory for non‑zero lengths */
    if (len_a > 0) {
        array_a = malloc(len_a);
        __CPROVER_assume(array_a != NULL);
    } else {
        /* When length is zero the pointer may be NULL or any value */
        array_a = NULL;
    }

    if (len_b > 0) {
        array_b = malloc(len_b);
        __CPROVER_assume(array_b != NULL);
    } else {
        array_b = NULL;
    }

    /* 4. Apply the function’s preconditions */
    __CPROVER_assume((len_a == 0) || AWS_MEM_IS_READABLE(array_a, len_a));
    __CPROVER_assume((len_b == 0) || AWS_MEM_IS_READABLE(array_b, len_b));

    /* 5. Save old state for immutability checks */
    const void *old_a = array_a;
    const void *old_b = array_b;
    size_t old_len_a = len_a;
    size_t old_len_b = len_b;

    /* 6. Call the function under test */
    bool result = aws_array_eq(array_a, len_a, array_b, len_b);

    /* 7. Compute the specification‑derived expected result */
    bool expected;
    if (len_a != len_b) {
        expected = false;
    } else if (len_a == 0) {
        expected = true;
    } else {
        expected = (memcmp(array_a, array_b, len_a) == 0);
    }

    /* 8. Assert that the implementation matches the specification */
    assert(result == expected);

    /* 9. Assert that inputs are unchanged (they are const) */
    assert(array_a == old_a);
    assert(array_b == old_b);
    assert(len_a == old_len_a);
    assert(len_b == old_len_b);
}
