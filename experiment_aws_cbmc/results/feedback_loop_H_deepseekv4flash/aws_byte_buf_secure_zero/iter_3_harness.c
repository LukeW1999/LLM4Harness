#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdlib.h>

#define MAX_BUFFER_SIZE 10

void aws_byte_buf_secure_zero_harness() {
    /* Nondeterministic choice: buf can be NULL or a valid pointer */
    struct aws_byte_buf *buf = malloc(sizeof(struct aws_byte_buf));
    int buf_is_null;
    __CPROVER_assume(buf_is_null == 0 || buf_is_null == 1);
    if (buf_is_null) {
        free(buf);
        buf = NULL;
    }

    /* If buf is non-null, make it a valid aws_byte_buf */
    if (buf != NULL) {
        /* Nondeterministic capacity and len */
        size_t capacity;
        size_t len;
        __CPROVER_assume(capacity <= MAX_BUFFER_SIZE);
        __CPROVER_assume(len <= capacity);
        buf->capacity = capacity;
        buf->len = len;

        /* Nondeterministic buffer: either NULL or allocated */
        int buffer_is_null;
        __CPROVER_assume(buffer_is_null == 0 || buffer_is_null == 1);
        if (buffer_is_null) {
            buf->buffer = NULL;
        } else {
            buf->buffer = malloc(capacity);
            if (buf->buffer != NULL) {
                /* Fill buffer with nondeterministic values */
                for (size_t i = 0; i < capacity; i++) {
                    buf->buffer[i] = nondet_uchar();
                }
            }
        }

        /* Nondeterministic allocator */
        buf->allocator = nondet_allocator();

        /* Ensure validity: if capacity > 0, buffer must be non-null */
        __CPROVER_assume(aws_byte_buf_is_valid(buf));
    }

    /* Save old state if buf is non-null */
    struct aws_byte_buf old_buf;
    if (buf != NULL) {
        old_buf = *buf;
    }

    /* Call the function */
    aws_byte_buf_secure_zero(buf);

    /* Postconditions */
    if (buf == NULL) {
        /* Nothing to assert */
    } else {
        /* len must be 0 */
        assert(buf->len == 0);

        /* capacity unchanged */
        assert(buf->capacity == old_buf.capacity);

        /* allocator unchanged */
        assert(buf->allocator == old_buf.allocator);

        /* buffer pointer unchanged */
        assert(buf->buffer == old_buf.buffer);

        /* If old buffer was non-null and old len > 0, those bytes must be zero */
        if (old_buf.buffer != NULL && old_buf.len > 0) {
            for (size_t i = 0; i < old_buf.len; i++) {
                assert(buf->buffer[i] == 0);
            }
        }

        /* buf remains a valid aws_byte_buf */
        assert(aws_byte_buf_is_valid(buf));
    }
}
