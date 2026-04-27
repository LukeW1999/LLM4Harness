#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_byte_buf_init_copy_from_cursor_harness() {
    struct aws_byte_buf dest;
    struct aws_allocator *allocator = aws_default_allocator();
    struct aws_byte_cursor src;

    // Assume preconditions
    __CPROVER_assume(aws_byte_cursor_is_valid(&src));
    __CPROVER_assume(src.ptr != NULL);
    __CPROVER_assume(src.len <= MAX_BUFFER_SIZE); // Define MAX_BUFFER_SIZE appropriately

    // Initialize dest to a known state
    dest.buffer = NULL;
    dest.len = 0;
    dest.capacity = 0;
    dest.allocator = NULL;

    // Save old state
    struct aws_byte_buf old_dest = dest;

    // Call function under test
    int result = aws_byte_buf_init_copy_from_cursor(&dest, allocator, src);

    // Assertions for success path
    if (result == AWS_OP_SUCCESS) {
        assert(dest.buffer != old_dest.buffer || src.len == 0); // buffer changes unless src.len is 0
        assert(dest.len == src.len); // len changes
        assert(dest.capacity >= src.len); // capacity changes
        assert(dest.allocator == allocator); // allocator changes
        if (src.len > 0) {
            assert_bytes_match(dest.buffer, src.ptr, src.len); // contents match
        }
    } else { // Assertions for failure path
        assert(dest.buffer == old_dest.buffer); // buffer unchanged
        assert(dest.len == old_dest.len); // len unchanged
        assert(dest.capacity == old_dest.capacity); // capacity unchanged
        assert(dest.allocator == old_dest.allocator); // allocator unchanged
    }

    // Validity invariants
    assert(aws_byte_cursor_is_valid(&src)); // src remains valid
    assert(aws_byte_buf_is_valid(&dest)); // dest is valid after call
}
