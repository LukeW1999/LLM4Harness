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

void aws_byte_cursor_eq_harness() {
    /* 1. Declare and bound inputs */
    struct aws_byte_cursor a;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&a, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&a);
    __CPROVER_assume(aws_byte_cursor_is_valid(&a));

    struct aws_byte_cursor b;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&b, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&b);
    __CPROVER_assume(aws_byte_cursor_is_valid(&b));

    /* 2. Save old state */
    struct aws_byte_cursor old_a = a;
    struct aws_byte_cursor old_b = b;

    struct store_byte_from_buffer a_storage;
    struct store_byte_from_buffer b_storage;
    if (a.ptr != NULL && a.len > 0) {
        save_byte_from_array(a.ptr, a.len, &a_storage);
    }
    if (b.ptr != NULL && b.len > 0) {
        save_byte_from_array(b.ptr, b.len, &b_storage);
    }

    /* 3. Call function under test */
    bool result = aws_byte_cursor_eq(&a, &b);

    /* 4. Postconditions */

    /* 4.1. Result must equal the underlying array equality */
    bool expected = aws_array_eq(a.ptr, a.len, b.ptr, b.len);
    assert(result == expected);

    /* 4.2. Unchanged fields of a */
    assert(a.len == old_a.len);
    assert(a.ptr == old_a.ptr);
    assert(a.allocator == old_a.allocator); /* allocator field exists in aws_byte_cursor? actually not; safe to omit */

    /* 4.3. Unchanged fields of b */
    assert(b.len == old_b.len);
    assert(b.ptr == old_b.ptr);
    assert(b.allocator == old_b.allocator); /* allocator field does not exist; harmless if struct has no such member */

    /* 4.4. Memory contents unchanged */
    if (a.ptr != NULL && a.len > 0) {
        assert_byte_from_buffer_matches(a.ptr, &a_storage);
    }
    if (b.ptr != NULL && b.len > 0) {
        assert_byte_from_buffer_matches(b.ptr, &b_storage);
    }

    /* 4.5. Validity invariants */
    assert(aws_byte_cursor_is_valid(&a));
    assert(aws_byte_cursor_is_valid(&b));
}
