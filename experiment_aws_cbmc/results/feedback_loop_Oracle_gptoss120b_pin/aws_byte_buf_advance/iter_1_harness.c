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

    /* nondeterministic initialization of the buffers */
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));
    __CPROVER_assume(aws_byte_buf_is_bounded(&output, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_valid(&output));

    /* nondeterministic length to advance, bounded */
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

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
    if (old_buf.capacity - old_buf.len >= len) {
        /* success case */
        assert(result);
        assert(buf.len == old_buf.len + len);
        assert(buf.capacity == old_buf.capacity);
        assert(buf.buffer == old_buf.buffer);
        assert(buf.allocator == old_buf.allocator);

        assert(output.capacity == len);
        assert(output.len == 0);
        if (old_buf.buffer == NULL) {
            assert(output.buffer == NULL);
        } else {
            assert(output.buffer == old_buf.buffer + old_buf.len);
        }
        assert(output.allocator == NULL);

        if (buf_mem_snapshot) {
            assert(memcmp(buf_mem_snapshot, buf.buffer, buf.capacity) == 0);
        }
    } else {
        /* failure case */
        assert(!result);
        assert(buf.len == old_buf.len);
        assert(buf.capacity == old_buf.capacity);
        assert(buf.buffer == old_buf.buffer);
        assert(buf.allocator == old_buf.allocator);

        assert(output.buffer == NULL);
        assert(output.len == 0);
        assert(output.capacity == 0);
        assert(output.allocator == NULL);

        if (buf_mem_snapshot) {
            assert(memcmp(buf_mem_snapshot, buf.buffer, buf.capacity) == 0);
        }
    }

    return 0;
}
