#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_BUFFER_SIZE 256

uint8_t nondet_uint8_t(void);
size_t nondet_size_t(void);

void aws_array_eq_harness(void) {
    /* nondeterministic lengths */
    size_t lhs_len = nondet_size_t();
    size_t rhs_len = nondet_size_t();

    /* ground‑truth preconditions */
    __CPROVER_assume(lhs_len <= MAX_BUFFER_SIZE);
    __CPROVER_assume(rhs_len <= MAX_BUFFER_SIZE);

    /* allocate buffers */
    uint8_t *lhs = NULL;
    uint8_t *rhs = NULL;

    if (lhs_len > 0) {
        lhs = malloc(lhs_len);
        __CPROVER_assume(lhs != NULL);
        for (size_t i = 0; i < lhs_len; ++i) {
            lhs[i] = nondet_uint8_t();
        }
    }

    if (rhs_len > 0) {
        rhs = malloc(rhs_len);
        __CPROVER_assume(rhs != NULL);
        for (size_t i = 0; i < rhs_len; ++i) {
            rhs[i] = nondet_uint8_t();
        }
    }

    __CPROVER_assume((lhs_len == 0) || AWS_MEM_IS_READABLE(lhs, lhs_len));
    __CPROVER_assume((rhs_len == 0) || AWS_MEM_IS_READABLE(rhs, rhs_len));

    /* make copies for frame condition checks */
    uint8_t *lhs_copy = NULL;
    uint8_t *rhs_copy = NULL;

    if (lhs_len > 0) {
        lhs_copy = malloc(lhs_len);
        __CPROVER_assume(lhs_copy != NULL);
        memcpy(lhs_copy, lhs, lhs_len);
    }

    if (rhs_len > 0) {
        rhs_copy = malloc(rhs_len);
        __CPROVER_assume(rhs_copy != NULL);
        memcpy(rhs_copy, rhs, rhs_len);
    }

    /* call the function under verification */
    bool result = aws_array_eq(lhs, lhs_len, rhs, rhs_len);

    /* functional postcondition */
    bool expected;
    if (lhs_len != rhs_len) {
        expected = false;
    } else if (lhs_len == 0) {
        expected = true;
    } else {
        expected = (memcmp(lhs, rhs, lhs_len) == 0);
    }
    assert(result == expected);

    /* frame condition: inputs must remain unchanged */
    if (lhs_len > 0) {
        assert(memcmp(lhs, lhs_copy, lhs_len) == 0);
    }
    if (rhs_len > 0) {
        assert(memcmp(rhs, rhs_copy, rhs_len) == 0);
    }

    /* clean up */
    free(lhs);
    free(rhs);
    free(lhs_copy);
    free(rhs_copy);
    return 0;
}
