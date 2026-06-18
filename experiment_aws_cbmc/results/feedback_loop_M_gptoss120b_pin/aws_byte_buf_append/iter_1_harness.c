#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

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

    /* 3. Save old state of the destination buffer */
    struct aws_byte_buf old_to = to;
    struct store_byte_from_buffer old_byte;
    save_byte_from_array(to.buffer, to.capacity, &old_byte);

    /* 4. Call the function under test */
    int result = aws_byte_buf_append(&to, &from);

    /* 5. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* Changed fields */
        assert(to.len == old_to.len + from.len);
        if (from.len > 0) {
            assert_bytes_match(to.buffer + old_to.len, from.ptr, from.len);
        }

        /* Unchanged fields */
        assert(to.capacity == old_to.capacity);
        assert(to.allocator == old_to.allocator);
        assert(to.buffer == old_to.buffer);

        /* Bytes outside the copied region remain unchanged */
        size_t idx = nondet_size_t();
        __CPROVER_assume(idx < to.capacity);
        __CPROVER_assume(idx < old_to.len || idx >= old_to.len + from.len);
        assert(to.buffer[idx] == old_to.buffer[idx]);
    } else {
        /* On failure the destination buffer must be unchanged */
        assert(to.len == old_to.len);
        assert(to.capacity == old_to.capacity);
        assert(to.allocator == old_to.allocator);
        assert(to.buffer == old_to.buffer);
        assert_byte_from_buffer_matches(to.buffer, &old_byte);
    }

    /* 6. Validity invariants */
    assert(aws_byte_buf_is_valid(&to));
    assert(aws_byte_cursor_is_valid(&from));
}
