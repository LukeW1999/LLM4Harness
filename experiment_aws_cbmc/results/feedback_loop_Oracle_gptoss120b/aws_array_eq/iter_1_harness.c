#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <aws/common/common.h>
#include <aws/common/byte_buf.h>

/* Maximum size for the nondeterministic buffers */
#define MAX_BUFFER_SIZE 1024

void aws_array_eq_harness(void) {
    /* Nondeterministic lengths */
    size_t lhs_len = nondet_size_t();
    size_t rhs_len = nondet_size_t();

    __CPROVER_assume(lhs_len <= MAX_BUFFER_SIZE);
    __CPROVER_assume(rhs_len <= MAX_BUFFER_SIZE);

    /* Allocate buffers */
    uint8_t *lhs = NULL;
    uint8_t *rhs = NULL;

    if (lhs_len > 0) {
        lhs = malloc(lhs_len);
        __CPROVER_assume(lhs != NULL);
    }
    if (rhs_len > 0) {
        rhs = malloc(rhs_len);
        __CPROVER_assume(rhs != NULL);
    }

    /* Assume the buffers are readable (as required by the function) */
    __CPROVER_assume((lhs_len == 0) || AWS_MEM_IS_READABLE(lhs, lhs_len));
    __CPROVER_assume((rhs_len == 0) || AWS_MEM_IS_READABLE(rhs, rhs_len));

    /* Initialize buffers with nondeterministic data */
    if (lhs_len > 0) {
        for (size_t i = 0; i < lhs_len; ++i) {
            lhs[i] = nondet_uint8_t();
        }
    }
    if (rhs_len > 0) {
        for (size_t i = 0; i < rhs_len; ++i) {
            rhs[i] = nondet_uint8_t();
        }
    }

    /* Preserve original contents for frame condition checks */
    uint8_t *lhs_orig = NULL;
    uint8_t *rhs_orig = NULL;
    if (lhs_len > 0) {
        lhs_orig = malloc(lhs_len);
        __CPROVER_assume(lhs_orig != NULL);
        memcpy(lhs_orig, lhs, lhs_len);
    }
    if (rhs_len > 0) {
        rhs_orig = malloc(rhs_len);
        __CPROVER_assume(rhs_orig != NULL);
        memcpy(rhs_orig, rhs, rhs_len);
    }

    /* Call the function under test */
    bool result = aws_array_eq(lhs, lhs_len, rhs, rhs_len);

    /* Compute the expected result according to the specification */
    bool expected;
    if (lhs_len != rhs_len) {
        expected = false;
    } else if (lhs_len == 0) {
        expected = true;
    } else {
        expected = (memcmp(lhs, rhs, lhs_len) == 0);
    }

    /* Postcondition: return value must match the specification */
    assert(result == expected);

    /* Frame condition: input buffers must remain unchanged */
    if (lhs_len > 0) {
        assert(memcmp(lhs, lhs_orig, lhs_len) == 0);
    }
    if (rhs_len > 0) {
        assert(memcmp(rhs, rhs_orig, rhs_len) == 0);
    }

    /* Clean up */
    free(lhs);
    free(rhs);
    free(lhs_orig);
    free(rhs_orig);

    return 0;
}
