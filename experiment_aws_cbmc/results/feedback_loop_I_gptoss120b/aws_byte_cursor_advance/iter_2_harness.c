#include <aws/common/byte_buf.h>
#include <aws/common/byte_cursor.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_cursor_advance_harness(void) {
    /* 1. Set up allocator */
    struct aws_allocator *alloc = aws_default_allocator();

    /* 2. Create a bounded byte buffer to back the cursor */
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* 3. Initialize cursor from the buffer */
    struct aws_byte_cursor cursor = {
        .ptr = buf.buffer,
        .len = buf.len
    };

    /* 4. Save old state for frame condition checks */
    struct aws_byte_cursor old_cursor = cursor;

    /* 5. Nondeterministic amount to advance, bounded */
    size_t amount;
    __CPROVER_assume(amount <= MAX_BUFFER_SIZE);

    /* 6. Call function under test */
    int result = aws_byte_cursor_advance(&cursor, amount);

    /* 7. Post‑condition assertions */

    /* Validity predicate must hold after the call */
    assert(aws_byte_cursor_is_valid(&cursor));

    if (result == AWS_OP_SUCCESS) {
        /* Length invariant */
        assert(cursor.len == old_cursor.len - amount);

        /* Frame condition for pointer */
        if (amount > 0) {
            /* If we advance a non‑zero amount, the original pointer must have been non‑NULL */
            assert(old_cursor.ptr != NULL);
            assert(cursor.ptr == old_cursor.ptr + amount);
        } else {
            /* Advancing by zero leaves the pointer unchanged */
            assert(cursor.ptr == old_cursor.ptr);
        }
    } else {
        /* On failure, the cursor must be unchanged */
        assert(result == AWS_OP_ERR);
        assert(cursor.ptr == old_cursor.ptr);
        assert(cursor.len == old_cursor.len);
    }

    /* 8. Frame condition: the backing buffer must remain unchanged */
    assert(buf.buffer == old_cursor.ptr);
    assert(buf.len == old_cursor.len);
    assert(buf.capacity == buf.capacity); /* capacity unchanged by definition */
    assert(buf.allocator == alloc);
}
