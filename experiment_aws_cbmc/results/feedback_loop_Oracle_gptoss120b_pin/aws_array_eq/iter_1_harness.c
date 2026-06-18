#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 256

void aws_array_eq_harness(void) {
    /* Allocate buffers with a generous maximum size */
    uint8_t *lhs = malloc(MAX_BUFFER_SIZE);
    uint8_t *rhs = malloc(MAX_BUFFER_SIZE);
    __CPROVER_assume(lhs != NULL);
    __CPROVER_assume(rhs != NULL);

    /* Nondeterministic lengths bounded by MAX_BUFFER_SIZE */
    size_t lhs_len = nondet_size_t();
    size_t rhs_len = nondet_size_t();
    __CPROVER_assume(lhs_len <= MAX_BUFFER_SIZE);
    __CPROVER_assume(rhs_len <= MAX_BUFFER_SIZE);

    /* Populate buffers with nondeterministic data */
    for (size_t i = 0; i < lhs_len; ++i) {
        lhs[i] = nondet_uint8_t();
    }
    for (size_t i = 0; i < rhs_len; ++i) {
        rhs[i] = nondet_uint8_t();
    }

    /* Preserve copies for frame condition checks */
    uint8_t lhs_copy[MAX_BUFFER_SIZE];
    uint8_t rhs_copy[MAX_BUFFER_SIZE];
    memcpy(lhs_copy, lhs, lhs_len);
    memcpy(rhs_copy, rhs, rhs_len);

    /* Apply the ground‑truth preconditions */
    __CPROVER_assume((lhs_len == 0) || AWS_MEM_IS_READABLE(lhs, lhs_len));
    __CPROVER_assume((rhs_len == 0) || AWS_MEM_IS_READABLE(rhs, rhs_len));

    /* Call the function under verification */
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

    /* Post‑condition: return value must match the specification */
    assert(result == expected);

    /* Frame condition: input buffers must remain unchanged */
    for (size_t i = 0; i < lhs_len; ++i) {
        assert(lhs[i] == lhs_copy[i]);
    }
    for (size_t i = 0; i < rhs_len; ++i) {
        assert(rhs[i] == rhs_copy[i]);
    }

    /* Clean up */
    free(lhs);
    free(rhs);
    return 0;
}
