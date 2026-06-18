#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_BUFFER_SIZE 1024

/* nondeterministic size_t */
size_t nondet_size_t(void);

void aws_byte_buf_advance_harness(void) {
    struct aws_byte_buf buf;
    struct aws_byte_buf output;
    size_t len;

    /* nondet capacity and initial length */
    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity <= MAX_BUFFER_SIZE);

    /* allocate underlying memory if capacity > 0 */
    uint8_t *buf_mem = NULL;
    if (capacity > 0) {
        buf_mem = malloc(capacity);
        __CPROVER_assume(buf_mem != NULL);
    }

    buf.buffer   = buf_mem;
    buf.capacity = capacity;
    buf.len      = nondet_size_t();
    __CPROVER_assume(buf.len <= capacity);
    buf.allocator = aws_default_allocator();

    /* output must be an empty, valid byte_buf */
    output.buffer   = NULL;
    output.capacity = 0;
    output.len      = 0;
    output.allocator = NULL;
    __CPROVER_assume(aws_byte_buf_is_valid(&output));

    /* advance length */
    len = nondet_size_t();
    __CPROVER_assume(len <= capacity - buf.len);
    if (len > 0) {
        __CPROVER_assume(buf.buffer != NULL);
    }

    /* snapshot old state */
    struct aws_byte_buf old_buf = buf;
    uint8_t *old_mem = NULL;
    if (buf.buffer != NULL && buf.capacity > 0) {
        old_mem = malloc(buf.capacity);
        __CPROVER_assume(old_mem != NULL);
        memcpy(old_mem, buf.buffer, buf.capacity);
    }

    bool ret = aws_byte_buf_advance(&buf, &output, len);

    if (ret) {
        __CPROVER_assert(buf.len == old_buf.len + len,
                         "buf.len increased by len on success");
        __CPROVER_assert(output.capacity == len,
                         "output.capacity equals len on success");
        __CPROVER_assert(output.len == 0,
                         "output.len is zero on success");
        __CPROVER_assert(output.buffer == old_buf.buffer + old_buf.len,
                         "output.buffer points to original buffer+old_len");
        __CPROVER_assert(output.allocator == NULL,
                         "output.allocator is NULL on success");
    } else {
        __CPROVER_assert(output.buffer == NULL,
                         "output.buffer is NULL on failure");
        __CPROVER_assert(output.len == 0,
                         "output.len is zero on failure");
        __CPROVER_assert(output.capacity == 0,
                         "output.capacity is zero on failure");
        __CPROVER_assert(output.allocator == NULL,
                         "output.allocator is NULL on failure");
        __CPROVER_assert(buf.len == old_buf.len,
                         "buf.len unchanged on failure");
        __CPROVER_assert(buf.buffer == old_buf.buffer,
                         "buf.buffer unchanged on failure");
        __CPROVER_assert(buf.capacity == old_buf.capacity,
                         "buf.capacity unchanged on failure");
        __CPROVER_assert(buf.allocator == old_buf.allocator,
                         "buf.allocator unchanged on failure");
    }

    /* frame condition: underlying memory unchanged */
    if (old_mem != NULL) {
        for (size_t i = 0; i < buf.capacity; ++i) {
            __CPROVER_assert(old_mem[i] == buf.buffer[i],
                             "buffer memory unchanged");
        }
        free(old_mem);
    }

    return 0;
}
