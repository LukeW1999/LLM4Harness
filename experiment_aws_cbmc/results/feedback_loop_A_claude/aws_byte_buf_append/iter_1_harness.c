#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "aws/common/byte_buf.h"
#include "proof_helpers/make_common_data_structures.h"

/*
 * Analysis before writing:
 * 1. Fields that change on SUCCESS:
 *    - to->len increases by from->len (data is copied)
 *    - to->buffer contents change (memcpy writes into it)
 * 2. Fields UNCHANGED (implicit invariants):
 *    - to->capacity (not modified)
 *    - to->allocator (not modified)
 *    - to->buffer pointer (not modified, only contents)
 *    - from->len (not modified)
 *    - from->ptr (not modified)
 * 3. On FAILURE (AWS_ERROR_DEST_COPY_TOO_SMALL):
 *    - to->len is unchanged
 *    - to->capacity is unchanged
 *    - to->buffer pointer is unchanged
 *    - to->allocator is unchanged
 * 4. Validity invariants:
 *    - aws_byte_buf_is_valid(to) holds after call
 *    - aws_byte_cursor_is_valid(from) holds after call
 */

void aws_byte_buf_append_harness() {
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

    /* from cursor must be completely unchanged */
    assert(from.len == old_from.len);
    assert(from.ptr == old_from.ptr);

    if (result == AWS_OP_SUCCESS) {
        /* On success: to->len increases by from->len */
        assert(to.len == old_to.len + old_from.len);
        /* The new len must be <= capacity */
        assert(to.len <= to.capacity);
    } else {
        /* On failure: to->len is unchanged */
        assert(to.len == old_to.len);
        /* Failure means there wasn't enough space */
        assert(to.capacity - old_to.len < old_from.len);
    }

    /* 6. Validity invariants must hold after the call */
    assert(aws_byte_buf_is_valid(&to));
    assert(aws_byte_cursor_is_valid(&from));
}
