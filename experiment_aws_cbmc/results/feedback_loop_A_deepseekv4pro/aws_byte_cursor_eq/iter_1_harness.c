#include <aws/common/byte_buf.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_cursor_eq_harness() {
    // 1. Declare and bound the two byte cursors
    struct aws_byte_cursor a;
    struct aws_byte_cursor b;

    // Bound the sizes to keep state space manageable
    __CPROVER_assume(aws_byte_cursor_is_bounded(&a, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_cursor_is_bounded(&b, MAX_BUFFER_SIZE));

    // Allocate backing buffers for both cursors
    ensure_byte_cursor_has_allocated_buffer_member(&a);
    ensure_byte_cursor_has_allocated_buffer_member(&b);

    // Assume the cursors are valid
    __CPROVER_assume(aws_byte_cursor_is_valid(&a));
    __CPROVER_assume(aws_byte_cursor_is_valid(&b));

    // 2. Save old state for immutability check
    struct aws_byte_cursor old_a = a;
    struct aws_byte_cursor old_b = b;

    // 3. Call the function under test
    bool result = aws_byte_cursor_eq(&a, &b);

    // 4. Assert immutability: no fields of either cursor may change
    //    The function is read-only, nothing should be modified.
    assert(a.len == old_a.len);
    assert(a.ptr == old_a.ptr);
    assert(b.len == old_b.len);
    assert(b.ptr == old_b.ptr);

    // 5. Assert postcondition: validity invariants are preserved
    assert(aws_byte_cursor_is_valid(&a));
    assert(aws_byte_cursor_is_valid(&b));

    // 6. Logical correctness: result must match the definition of equality
    size_t min_len = a.len < b.len ? a.len : b.len;
    bool mismatch_found = false;
    for (size_t i = 0; i < min_len; i++) {
        if (a.ptr[i] != b.ptr[i]) {
            mismatch_found = true;
            break;
        }
    }
    if (result) {
        assert(a.len == b.len && !mismatch_found);
    } else {
        assert(a.len != b.len || mismatch_found);
    }
}
