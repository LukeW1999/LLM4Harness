#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>

void aws_byte_buf_init_copy_from_cursor_harness() {
    struct aws_byte_buf dest;
    struct aws_allocator allocator;
    struct aws_byte_cursor src;

    // Initialize src with a valid aws_byte_cursor
    size_t src_len = nondet_size_t();
    __CPROVER_assume(src_len <= MAX_BUFFER_SIZE);
    uint8_t *src_ptr = malloc(src_len);
    __CPROVER_assume(src_ptr != NULL || src_len == 0);
    src.ptr = src_ptr;
    src.len = src_len;

    // Initialize allocator with a valid aws_allocator
    allocator.alloc = malloc;
    allocator.free = free;
    allocator.realloc = realloc;
    allocator.user_data = NULL;

    // Initialize dest with a default state
    dest.buffer = NULL;
    dest.len = 0;
    dest.capacity = 0;
    dest.allocator = NULL;

    // Save old state of dest
    struct aws_byte_buf old_dest = dest;

    // Call the function under test
    int result = aws_byte_buf_init_copy_from_cursor(&dest, &allocator, src);

    // Step 1: Success Path Assertions
    if (result == AWS_OP_SUCCESS) {
        assert(dest.buffer != NULL);
        assert(dest.len == src.len);
        assert(dest.capacity >= src.len);
        assert(dest.allocator == &allocator);
        assert(aws_byte_buf_is_valid(&dest));
        assert_bytes_match(dest.buffer, src.ptr, src.len);
    }

    // Step 2: Failure Path Assertions
    if (result == AWS_OP_ERR) {
        assert(dest.buffer == old_dest.buffer);
        assert(dest.len == old_dest.len);
        assert(dest.capacity == old_dest.capacity);
        assert(dest.allocator == old_dest.allocator);
        assert(aws_byte_buf_is_valid(&dest));
    }

    // Step 3: Frame Conditions Assertions
    assert(src.ptr == src_ptr);
    assert(src.len == src_len);
    assert(allocator.alloc == malloc);
    assert(allocator.free == free);
    assert(allocator.realloc == realloc);
    assert(allocator.user_data == NULL);

    // Step 4: Validity Invariants Assertions
    assert(aws_byte_cursor_is_valid(&src));
    assert(aws_byte_buf_is_valid(&dest));

    // Free allocated memory
    if (src_ptr != NULL) {
        free(src_ptr);
    }
    if (dest.buffer != NULL) {
        free(dest.buffer);
    }
}
