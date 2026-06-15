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

    /* 3. Save old state of the destination buffer */
    struct aws_byte_buf old_to = to;

    /* 4. Call the function under test */
    int result = aws_byte_buf_append(&to, &from);

    /* 5. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* Length must increase by the amount copied */
        assert(to.len == old_to.len + from.len);

        /* Buffer pointer, capacity and allocator must stay the same */
        assert(to.buffer == old_to.buffer);
        assert(to.capacity == old_to.capacity);
        assert(to.allocator == old_to.allocator);

        /* The bytes that were copied must match the source */
        if (from.len > 0) {
            assert_bytes_match(to.buffer + old_to.len, from.ptr, from.len);
        }
    } else {
        /* On failure the destination buffer must be unchanged */
        assert(to.len == old_to.len);
        assert(to.buffer == old_to.buffer);
        assert(to.capacity == old_to.capacity);
        assert(to.allocator == old_to.allocator);
    }

    /* 6. Fields of the source cursor must remain unchanged */
    assert(from.len == from.len);
    assert(from.ptr == from.ptr);

    /* 7. Validity invariants must hold after the call */
    assert(aws_byte_buf_is_valid(&to));
    assert(aws_byte_cursor_is_valid(&from));
}
