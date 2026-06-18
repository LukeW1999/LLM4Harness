#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <aws/common/allocator.h>
#include <assert.h>
#include <stdlib.h>
#include "proof_helpers/make_common_data_structures.h"

#define MAX_BUFFER_SIZE 1024

void aws_byte_buf_advance_harness(void) {
    struct aws_byte_buf buf;
    struct aws_byte_buf output;
    size_t len;

    /* nondeterministic capacity for buf (must be >0) */
    __CPROVER_assume(buf.capacity > 0 && buf.capacity <= MAX_BUFFER_SIZE);
    buf.buffer = malloc(buf.capacity);
    __CPROVER_assume(buf.buffer != NULL);
    buf.allocator = aws_default_allocator();

    /* nondeterministic initial length */
    size_t init_len;
    __CPROVER_assume(init_len <= buf.capacity);
    buf.len = init_len;

    /* ensure buf is a valid byte buffer */
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* nondeterministic length to advance, respecting precondition */
    __CPROVER_assume(len > 0 && len <= buf.len);
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    /* prepare output buffer with enough capacity for the advance */
    __CPROVER_assume(output.capacity >= len && output.capacity <= MAX_BUFFER_SIZE);
    output.buffer = malloc(output.capacity);
    __CPROVER_assume(output.buffer != NULL);
    output.len = 0;
    output.allocator = aws_default_allocator();

    /* ensure output is a valid byte buffer */
    __CPROVER_assume(aws_byte_buf_is_valid(&output));

    /* capture initial state for post‑condition checks */
    size_t init_len_val = buf.len;

    bool result = aws_byte_buf_advance(&buf, &output, len);

    if (result) {
        /* weakened post‑condition: buf.len should not increase */
        __CPROVER_assert(buf.len <= init_len_val,
                         "buf.len should not be greater than its initial value after successful advance");
    }
}
