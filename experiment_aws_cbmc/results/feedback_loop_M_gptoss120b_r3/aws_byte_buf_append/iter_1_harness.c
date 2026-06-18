#include <aws/common/byte_buf.h>
#include <aws/common/array_list.h>
#include <aws/common/linked_list.h>
#include <aws/common/math.h>
#include <aws/common/string.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_append_harness(void) {
    /* 1. Declare and bound the destination buffer */
    struct aws_byte_buf to;
    __CPROVER_assume(aws_byte_buf_is_bounded(&to, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&to);
    __CPROVER_assume(aws_byte_buf_is_valid(&to));

    /* 2. Declare and bound the source cursor */
    struct aws_byte_cursor from;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&from, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&from);
    __CPROVER_assume(aws_byte_cursor_is_valid(&from));

    /* 3. Save old state */
    struct aws_byte_buf old_to = to;
    struct aws_byte_cursor old_from = from;

    struct store_byte_from_buffer old_buf_content;
    if (to.buffer != NULL && to.capacity > 0) {
        save_byte_from_array(to.buffer, to.capacity, &old_buf_content);
    }

    /* 4. Call the function under test */
    int result = aws_byte_buf_append(&to, &from);

    /* 5. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* changed fields */
        assert(to.len == old_to.len + from.len);
        /* copied data matches source */
        if (from.len > 0) {
            assert_bytes_match(to.buffer + old_to.len, from.ptr, from.len);
        }
        /* unchanged fields of the destination buffer */
        assert(to.capacity == old_to.capacity);
        assert(to.allocator == old_to.allocator);
        assert(to.buffer == old_to.buffer);
        /* unchanged prefix of the destination buffer */
        if (old_to.len > 0) {
            assert_bytes_match(to.buffer, old_to.buffer, old_to.len);
        }
    } else {
        /* on failure the destination buffer must be unchanged */
        assert(to.len == old_to.len);
        assert(to.capacity == old_to.capacity);
        assert(to.allocator == old_to.allocator);
        assert(to.buffer == old_to.buffer);
        if (old_to.len > 0) {
            assert_bytes_match(to.buffer, old_to.buffer, old_to.len);
        }
    }

    /* 6. The source cursor must remain unchanged */
    assert(from.len == old_from.len);
    assert(from.ptr == old_from.ptr);

    /* 7. Validity invariants */
    assert(aws_byte_buf_is_valid(&to));
    assert(aws_byte_cursor_is_valid(&from));
}
