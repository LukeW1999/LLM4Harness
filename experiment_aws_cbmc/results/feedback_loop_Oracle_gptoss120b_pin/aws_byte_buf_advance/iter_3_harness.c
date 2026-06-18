#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "proof_helpers/make_common_data_structures.h"

#define MAX_BUFFER_SIZE 1024

void aws_byte_buf_advance_harness(void) {
    struct aws_byte_buf buf;
    struct aws_byte_buf output;
    size_t len;

    /* nondeterministic capacity and allocation for buf */
    __CPROVER_assume(buf.capacity <= MAX_BUFFER_SIZE);
    if (buf.capacity > 0) {
        buf.buffer = malloc(buf.capacity);
        __CPROVER_assume(buf.buffer != NULL);
    } else {
        buf.buffer = NULL;
    }
    __CPROVER_assume(buf.len <= buf.capacity);
    buf.allocator = NULL; /* external memory */

    /* ensure buf is a valid byte buffer */
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* nondeterministic length to advance, respecting precondition */
    __CPROVER_assume(len <= buf.len);
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    /* prepare output as a valid buffer with writable memory of at least len bytes */
    if (len > 0) {
        output.buffer = malloc(len);
        __CPROVER_assume(output.buffer != NULL);
    } else {
        output.buffer = NULL;
    }
    output.capacity = len;
    output.len = 0;
    output.allocator = NULL;
    __CPROVER_assume(aws_byte_buf_is_valid(&output));

    /* snapshot of the original buffer memory */
    uint8_t *buf_mem_snapshot = NULL;
    if (buf.buffer != NULL && buf.capacity > 0) {
        buf_mem_snapshot = malloc(buf.capacity);
        __CPROVER_assume(buf_mem_snapshot != NULL);
        memcpy(buf_mem_snapshot, buf.buffer, buf.capacity);
    }

    /* snapshot of the original buffer struct */
    struct aws_byte_buf old_buf = buf;

    bool result = aws_byte_buf_advance(&buf, &output, len);

    /* post‑conditions */
    assert(result);
    assert(buf.len == old_buf.len - len);
    assert(buf.capacity == old_buf.capacity - len);
    assert(buf.buffer == old_buf.buffer + len);
    assert(buf.allocator == old_buf.allocator);

    assert(output.buffer == old_buf.buffer);
    assert(output.capacity == len);
    assert(output.len == len);
    assert(output.allocator == NULL);

    if (buf_mem_snapshot) {
        assert(memcmp(buf_mem_snapshot, old_buf.buffer, old_buf.capacity) == 0);
    }
}
