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

void aws_byte_cursor_eq_harness(void) {
    /* 1. Declare and bound the two cursors */
    struct aws_byte_cursor a;
    struct aws_byte_cursor b;

    __CPROVER_assume(aws_byte_cursor_is_bounded(&a, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_cursor_is_bounded(&b, MAX_BUFFER_SIZE));

    ensure_byte_cursor_has_allocated_buffer_member(&a);
    ensure_byte_cursor_has_allocated_buffer_member(&b);

    __CPROVER_assume(aws_byte_cursor_is_valid(&a));
    __CPROVER_assume(aws_byte_cursor_is_valid(&b));

    /* 2. Save old state for immutability checks */
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

    /* 3. Call the function under test */
    bool result = aws_byte_cursor_eq(&a, &b);

    /* 4. Postconditions */

    /* 4a. Return value must equal the underlying array equality */
    assert(result == aws_array_eq(a.ptr, a.len, b.ptr, b.len));

    /* 4b. Inputs must remain unchanged */
    assert(a.len == old_a.len);
    assert(a.ptr == old_a.ptr);
    assert(b.len == old_b.len);
    assert(b.ptr == old_b.ptr);

    /* 4c. Memory contents must remain unchanged */
    if (a.ptr != NULL && a.len > 0) {
        assert_byte_from_buffer_matches(a.ptr, &a_storage);
    }
    if (b.ptr != NULL && b.len > 0) {
        assert_byte_from_buffer_matches(b.ptr, &b_storage);
    }

    /* 4d. Validity invariants must still hold */
    assert(aws_byte_cursor_is_valid(&a));
    assert(aws_byte_cursor_is_valid(&b));
}
