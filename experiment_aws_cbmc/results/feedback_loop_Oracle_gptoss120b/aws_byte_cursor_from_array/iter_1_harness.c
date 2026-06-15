#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <aws/common/byte_order.h>
#include <aws/common/allocator.h>
#include <aws/common/assert.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* Maximum size for the nondeterministic buffer */
#define MAX_BUF_SIZE 64

void aws_byte_cursor_from_array_harness(void) {
    /* Allocate a buffer that will serve as the source array */
    uint8_t *src_buf = malloc(MAX_BUF_SIZE);
    __CPROVER_assume(src_buf != NULL);

    /* Fill the buffer with nondeterministic data */
    for (size_t i = 0; i < MAX_BUF_SIZE; ++i) {
        src_buf[i] = (uint8_t) __CPROVER_nondet_uint();
    }

    /* Preserve a copy of the original buffer for frame condition checking */
    uint8_t src_buf_old[MAX_BUF_SIZE];
    for (size_t i = 0; i < MAX_BUF_SIZE; ++i) {
        src_buf_old[i] = src_buf[i];
    }

    /* Nondeterministic length, bounded by the allocated buffer size */
    size_t len = __CPROVER_nondet_size_t();
    __CPROVER_assume(len <= MAX_BUF_SIZE);

    /* The pointer passed to the function. For simplicity we always pass the allocated buffer. */
    const void *bytes = src_buf;

    /* Call the function under test */
    struct aws_byte_cursor cur = aws_byte_cursor_from_array(bytes, len);

    /* 1. Return value / error code correctness (validity predicates) */
    __CPROVER_assert(aws_byte_cursor_is_valid(&cur),
                     "aws_byte_cursor_is_valid should hold for the returned cursor");

    /* 2. Output buffer length/capacity invariants (length invariants) */
    __CPROVER_assert(cur.ptr == (uint8_t *)bytes,
                     "Returned cursor pointer must equal the input pointer");
    __CPROVER_assert(cur.len == len,
                     "Returned cursor length must equal the input length");

    /* 3. Memory not modified beyond the function's contract (frame conditions) */
    for (size_t i = 0; i < MAX_BUF_SIZE; ++i) {
        __CPROVER_assert(src_buf[i] == src_buf_old[i],
                         "Source buffer must remain unchanged after aws_byte_cursor_from_array");
    }

    /* Clean up */
    free(src_buf);
    return 0;
}
