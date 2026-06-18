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

void aws_byte_cursor_eq_byte_buf_harness(void) {
    /* 1. Declare and bound data structures */
    struct aws_byte_cursor a;
    struct aws_byte_buf b;

    __CPROVER_assume(aws_byte_cursor_is_bounded(&a, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_bounded(&b, MAX_BUFFER_SIZE));

    ensure_byte_cursor_has_allocated_buffer_member(&a);
    ensure_byte_buf_has_allocated_buffer_member(&b);

    __CPROVER_assume(aws_byte_cursor_is_valid(&a));
    __CPROVER_assume(aws_byte_buf_is_valid(&b));

    /* 2. Save old state before calling */
    struct aws_byte_cursor old_a = a;
    struct aws_byte_buf old_b = b;

    struct store_byte_from_buffer storage_a;
    struct store_byte_from_buffer storage_b;

    if (a.ptr != NULL && a.len > 0) {
        save_byte_from_array(a.ptr, a.len, &storage_a);
    }
    if (b.buffer != NULL && b.len > 0) {
        save_byte_from_array(b.buffer, b.len, &storage_b);
    }

    /* 3. Call function under test */
    bool result = aws_byte_cursor_eq_byte_buf(&a, &b);

    /* 4. Assert postconditions */
    /* Return value must match the underlying array equality function */
    bool expected = aws_array_eq(a.ptr, a.len, b.buffer, b.len);
    assert(result == expected);

    /* Unchanged fields for cursor */
    assert(a.ptr == old_a.ptr);
    assert(a.len == old_a.len);
    /* Unchanged fields for buffer */
    assert(b.buffer == old_b.buffer);
    assert(b.len == old_b.len);
    assert(b.capacity == old_b.capacity);
    assert(b.allocator == old_b.allocator);

    /* Memory contents unchanged */
    if (a.ptr != NULL && a.len > 0) {
        assert_byte_from_buffer_matches(a.ptr, &storage_a);
    }
    if (b.buffer != NULL && b.len > 0) {
        assert_byte_from_buffer_matches(b.buffer, &storage_b);
    }

    /* 5. Validity invariants must still hold */
    assert(aws_byte_cursor_is_valid(&a));
    assert(aws_byte_buf_is_valid(&b));
}
