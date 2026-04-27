#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_byte_buf_init_copy_harness() {
    struct aws_byte_buf dest;
    struct aws_byte_buf src;
    struct aws_allocator *allocator = aws_default_allocator();

    // Initialize src with arbitrary values
    src.len = nondet_size_t();
    src.capacity = src.len; // Ensure capacity is at least as large as len for valid byte buf
    src.buffer = can_fail_malloc(src.capacity);
    src.allocator = allocator;

    // Initialize dest with arbitrary values
    dest.len = nondet_size_t();
    dest.capacity = nondet_size_t();
    dest.buffer = can_fail_malloc(dest.capacity);
    dest.allocator = allocator;

    // Store old state of dest
    struct aws_byte_buf old_dest = dest;

    // Assume src is valid
    assume(aws_byte_buf_is_valid(&src));

    // Assume allocator's mem_acquire is set correctly
    __CPROVER_assume(allocator->mem_acquire != NULL);

    // Call the function under test
    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    if (result == AWS_OP_SUCCESS) {
        // Assertions for success path
        assert(dest.len == src.len);
        assert(dest.capacity >= src.len); // Capacity should be at least as large as len
        assert(dest.allocator == allocator);
        assert(dest.buffer != NULL);
        assert_bytes_match(dest.buffer, src.buffer, src.len);
        assert(aws_byte_buf_is_valid(&dest));
    } else {
        // Assertions for failure path
        assert(dest.len == old_dest.len);
        assert(dest.buffer == NULL);
        assert(dest.capacity == old_dest.capacity);
        assert(dest.allocator == allocator);
        assert(aws_byte_buf_is_valid(&dest));
    }

    // Free allocated memory
    if (src.buffer) {
        free(src.buffer);
    }
    if (dest.buffer && dest.buffer != src.buffer) {
        free(dest.buffer);
    }
}
