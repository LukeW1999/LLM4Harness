#include <stddef.h>
#include <stdbool.h>
#include <assert.h>
#include <stdint.h>
#include <limits.h>
#include "aws/common/common.h"

/* Nondeterministic helper provided by CBMC */
size_t nondet_size_t(void);

/* Function under test */
bool aws_add_size_saturating(size_t a, size_t b, size_t *result);

void aws_add_size_saturating_harness(void) {
    /* 1. Nondeterministic inputs */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    /* 2. Result storage */
    size_t result = 0;

    /* 3. Call the function under test */
    bool ok = aws_add_size_saturating(a, b, &result);

    /* 4. Expected behaviour according to the actual implementation:
       - Returns false on overflow (saturation occurred)
       - Returns true when addition succeeds without overflow
    */
    bool overflow = a > SIZE_MAX - b;
    size_t expected = overflow ? SIZE_MAX : a + b;
    bool expected_ok = !overflow;  /* true if no overflow, false otherwise */

    /* 5. Verify */
    assert(ok == expected_ok);
    assert(result == expected);
}
