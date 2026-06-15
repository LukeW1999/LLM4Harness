#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <limits.h>

#include "proof_helpers/make_common_data_structures.h"

/* Function under test */
int aws_add_size_saturating(size_t a, size_t b, size_t *result);

void aws_add_size_saturating_harness(void) {
    /* 1. Non‑deterministic inputs, bounded */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    /* Allocate space for the output */
    size_t *out = malloc(sizeof(size_t));
    __CPROVER_assume(out != NULL);

    /* 2. Force both overflow and non‑overflow paths to be reachable */
    if (nondet_bool()) {
        /* Force overflow: a + b would wrap */
        __CPROVER_assume(a > SIZE_MAX - b);
    } else {
        /* No overflow */
        __CPROVER_assume(a <= SIZE_MAX - b);
    }

    /* 3. Call the function under test */
    int rc = aws_add_size_saturating(a, b, out);

    /* 4. Post‑conditions */
    if (a > SIZE_MAX - b) {
        /* Overflow case */
        assert(rc != 0);
        assert(*out == SIZE_MAX);
    } else {
        /* Normal addition case */
        assert(rc == 0);
        assert(*out == a + b);
    }

    /* 5. Clean up */
    free(out);
}
