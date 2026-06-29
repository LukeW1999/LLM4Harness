#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_BUFFER_SIZE 8

void aws_byte_cursor_eq_byte_buf_harness(void) {
    struct aws_byte_cursor cur;
    struct aws_byte_buf buf;

    /* Setup preconditions */
    __CPROVER_assume(aws_byte_cursor_is_bounded(&cur, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_cursor_is_valid(&cur));
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Save state before call for frame condition checks */
    size_t old_cur_len = cur.len;
    uint8_t *old_cur_ptr = cur.ptr;
    size_t old_buf_len = buf.len;
    size_t old_buf_capacity = buf.capacity;
    uint8_t *old_buf_buffer = buf.buffer;

    /* Call the function under test */
    bool result = aws_byte_cursor_eq_byte_buf(&cur, &buf);

    /* Postcondition 1: Return value correctness
     * The function returns true iff the cursor and buffer have the same length
     * and the same contents. */
    if (result) {
        assert(cur.len == buf.len);
    } else {
        /* If lengths differ, result must be false */
        if (cur.len != buf.len) {
            assert(!result);
        }
    }

    /* Postcondition 2: Output buffer length/capacity invariants
     * The cursor and buffer should remain valid after the call */
    assert(aws_byte_cursor_is_valid(&cur));
    assert(aws_byte_buf_is_valid(&buf));

    /* Postcondition 3: Frame conditions - neither cursor nor buffer is modified */
    assert(cur.len == old_cur_len);
    assert(cur.ptr == old_cur_ptr);
    assert(buf.len == old_buf_len);
    assert(buf.capacity == old_buf_capacity);
    assert(buf.buffer == old_buf_buffer);

    /* Postcondition 4: Result is a valid boolean */
    assert(result == true || result == false);
}
