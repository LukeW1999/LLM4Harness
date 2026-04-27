#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "aws/common/byte_buf.h"
#include "proof_helpers/make_common_data_structures.h"

/* AWS error codes */
#define AWS_ERROR_DEST_COPY_TOO_SMALL 0x0002
#define AWS_OP_SUCCESS 0
#define AWS_OP_ERR (-1)

/* Validity predicates */
bool aws_byte_buf_is_valid(const struct aws_byte_buf *buf) {
    return buf != NULL &&
           buf->len <= buf->capacity &&
           (buf->capacity == 0 || buf->buffer != NULL);
}

bool aws_byte_cursor_is_valid(const struct aws_byte_cursor *cursor) {
    return cursor != NULL &&
           (cursor->len == 0 || cursor->ptr != NULL);
}

void aws_byte_buf_append_harness(void) {
    /* 1. Declare and set up the destination buffer */
    struct aws_byte_buf to;
    __CPROVER_assume(aws_byte_buf_is_bounded(&to, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&to);
    __CPROVER_assume(aws_byte_buf_is_valid(&to));

    /* 2. Declare and set up the source cursor */
    struct aws_byte_cursor from;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&from, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&from);
    __CPROVER_assume(aws_byte_cursor_is_valid(&from));

    /* 3. Save old state before calling */
    struct aws_byte_buf old_to = to;
    struct aws_byte_cursor old_from = from;

    /* 4. Call the function under test */
    int result = aws_byte_buf_append(&to, &from);

    /* 5. Assert postconditions */

    /* Fields that must NEVER change regardless of result */
    assert(to.capacity == old_to.capacity);
    assert(to.allocator == old_to.allocator);
    assert(to.buffer == old_to.buffer);

    /* Cursor must never be modified */
    assert(from.len == old_from.len);
    assert(from.ptr == old_from.ptr);

    /* Both return paths */
    if (result == AWS_OP_SUCCESS) {
        /* On success: len increases by from->len */
        assert(to.len == old_to.len + old_from.len);
    } else {
        /* On failure: buffer was too small, to->len must be unchanged */
        assert(to.len == old_to.len);
        /* Failure condition: capacity - len < from->len */
        assert(old_to.capacity - old_to.len < old_from.len);
    }

    /* 6. Validity invariants must hold after the call */
    assert(aws_byte_buf_is_valid(&to));
    assert(aws_byte_cursor_is_valid(&from));
}

int main(void) {
    aws_byte_buf_append_harness();
    return 0;
}
