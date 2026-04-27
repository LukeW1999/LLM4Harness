#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <aws/common/byte_buf.h>
#include <aws/common/byte_cursor.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_append_harness() {
    struct aws_byte_buf to;
    struct aws_byte_cursor from;

    // Use a default allocator
    struct aws_allocator *allocator = aws_default_allocator();

    // Initialize to with arbitrary but valid values and ensure it has enough capacity
    size_t max_initial_len = 100;
    size_t max_append_len = 100;
    __CPROVER_assume(to.capacity >= max_initial_len + max_append_len);
    to.allocator = allocator;
    to.buffer = (uint8_t *)malloc(to.capacity);
    to.len = nondet_size_t();
    __CPROVER_assume(to.len <= max_initial_len);

    from.buffer = (uint8_t *)malloc(max_append_len);
    from.len = nondet_size_t();
    __CPROVER_assume(from.len <= max_append_len);

    // Save old state of to
    struct aws_byte_buf old_to = to;

    // Call the function under test
    int result = aws_byte_buf_append(&to, &from);

    // Postconditions
    if (result == AWS_OP_SUCCESS) {
        assert(to.len == old_to.len + from.len);
        assert_bytes_match(to.buffer + old_to.len, from.buffer, from.len);
    } else {
        assert(result == AWS_OP_ERR);
        assert(to.len == old_to.len);
        assert(to.buffer == old_to.buffer);
    }

    // Frame conditions
    assert(to.capacity == old_to.capacity);
    assert(to.allocator == old_to.allocator);
    assert(from.len == from.len); // Always unchanged
    assert(from.buffer == from.buffer); // Always unchanged

    // Validity invariants
    assert(aws_byte_buf_is_valid(&to));
    assert(aws_byte_cursor_is_valid(&from));

    // Free allocated memory
    free(to.buffer);
    free(from.buffer);
}
