#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "proof_helpers/make_common_data_structures.h"
#include "proof_helpers/proof_allocators.h"

void aws_byte_buf_append_harness() {
    struct aws_byte_buf to;
    struct aws_byte_cursor from;

    // Use a default allocator
    struct aws_allocator *allocator = aws_default_allocator();

    // Initialize to with arbitrary but valid values and ensure it has enough capacity
    __CPROVER_assume(to.capacity >= to.len + from.len);
    ensure_byte_buf_has_allocated_buffer_member(&to, allocator);
    ensure_byte_cursor_has_allocated_buffer_member(&from, allocator);

    // Save old state of to
    struct aws_byte_buf old_to = to;

    // Call the function under test
    int result = aws_byte_buf_append(&to, &from);

    // Postconditions
    if (result == AWS_OP_SUCCESS) {
        assert(to.len == old_to.len + from.len);
        assert_bytes_match(to.buffer + old_to.len, from.ptr, from.len);
    } else {
        assert(result == AWS_OP_ERR);
        assert(to.len == old_to.len);
        assert(to.buffer == old_to.buffer);
    }

    // Frame conditions
    assert(to.capacity == old_to.capacity);
    assert(to.allocator == old_to.allocator);
    assert(from.len == from.len); // Always unchanged
    assert(from.ptr == from.ptr); // Always unchanged

    // Validity invariants
    assert(aws_byte_buf_is_valid(&to));
    assert(aws_byte_cursor_is_valid(&from));
}
