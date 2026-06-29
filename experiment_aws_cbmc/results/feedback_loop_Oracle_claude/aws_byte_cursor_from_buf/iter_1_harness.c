#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 10
#endif

void aws_byte_cursor_from_buf_harness(void) {
    struct aws_byte_buf buf;

    /* Ground-truth preconditions */
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Save pre-call state for frame condition checks */
    size_t old_buf_len = buf.len;
    size_t old_buf_capacity = buf.capacity;
    uint8_t *old_buf_buffer = buf.buffer;
    struct aws_allocator *old_buf_allocator = buf.allocator;

    /* Call the function under test */
    struct aws_byte_cursor cursor = aws_byte_cursor_from_buf(&buf);

    /* Postcondition 1: The returned cursor must be valid */
    assert(aws_byte_cursor_is_valid(&cursor));

    /* Postcondition 2: The cursor's ptr must equal the buffer's buffer pointer */
    assert(cursor.ptr == buf.buffer);

    /* Postcondition 3: The cursor's len must equal the buffer's len */
    assert(cursor.len == buf.len);

    /* Postcondition 4: Length invariant - cursor len must not exceed buffer capacity */
    assert(cursor.len <= buf.capacity);

    /* Postcondition 5: Frame conditions - the input buffer must not be modified */
    assert(buf.len == old_buf_len);
    assert(buf.capacity == old_buf_capacity);
    assert(buf.buffer == old_buf_buffer);
    assert(buf.allocator == old_buf_allocator);

    /* Postcondition 6: If buffer pointer is NULL, cursor ptr is NULL and len is 0 */
    if (old_buf_buffer == NULL) {
        assert(cursor.ptr == NULL);
        assert(cursor.len == 0);
    }

    /* Postcondition 7: If buffer pointer is non-NULL, cursor ptr is non-NULL */
    if (old_buf_buffer != NULL) {
        assert(cursor.ptr != NULL);
    }
}
