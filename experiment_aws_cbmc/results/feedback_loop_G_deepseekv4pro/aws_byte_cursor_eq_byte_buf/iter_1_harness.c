#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

void aws_byte_cursor_eq_byte_buf_harness() {
    /* Declare and bound input structures */
    struct aws_byte_cursor a;
    struct aws_byte_buf b;

    __CPROVER_assume(aws_byte_cursor_is_bounded(&a, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&a);
    __CPROVER_assume(aws_byte_cursor_is_valid(&a));

    __CPROVER_assume(aws_byte_buf_is_bounded(&b, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&b);
    __CPROVER_assume(aws_byte_buf_is_valid(&b));

    /* Save old state before calling (const pointers, but verify immutability) */
    struct aws_byte_cursor old_a = a;
    struct aws_byte_buf old_b = b;

    /* Function under test */
    bool rv = aws_byte_cursor_eq_byte_buf(&a, &b);

    /* Postconditions */
    /* 1. Return value reflects equality of contents */
    bool equal = (a.len == b.len);
    if (equal && a.len > 0) {
        bool all_same = true;
        for (size_t i = 0; i < a.len; i++) {
            if (a.ptr[i] != b.buffer[i]) {
                all_same = false;
                break;
            }
        }
        equal = all_same;
    }

    if (rv) {
        assert(equal);
    } else {
        assert(!equal);
    }

    /* 2. Inputs must not be modified (all fields unchanged) */
    assert(a.len == old_a.len);
    assert(a.ptr == old_a.ptr);
    assert(b.len == old_b.len);
    assert(b.buffer == old_b.buffer);
    assert(b.capacity == old_b.capacity);
    assert(b.allocator == old_b.allocator);

    /* 3. Validity invariants remain true after the call */
    assert(aws_byte_cursor_is_valid(&a));
    assert(aws_byte_buf_is_valid(&b));
}
