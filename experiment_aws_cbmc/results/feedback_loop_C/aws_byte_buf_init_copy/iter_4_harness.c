#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

void aws_byte_buf_init_copy_harness() {
    struct aws_byte_buf dest;
    struct aws_allocator allocator;
    struct aws_byte_buf src;

    // Initialize src with arbitrary but valid values
    src.len = nondet_size_t();
    src.capacity = nondet_size_t();
    src.buffer = (uint8_t *)malloc(src.capacity);
    __CPROVER_assume(src.buffer != NULL || src.capacity == 0); // Ensure buffer is NULL if capacity is 0
    src.allocator = &allocator;

    // Initialize allocator with arbitrary but valid values
    allocator.alloc = malloc;
    allocator.free = free;

    // Save old state of dest
    struct aws_byte_buf old_dest = dest;

    // Call the function under test
    int result = aws_byte_buf_init_copy(&dest, &allocator, &src);

    // Success path assertions
    if (result == AWS_OP_SUCCESS) {
        assert(aws_byte_buf_is_valid(&dest));
        assert(dest.len == src.len);
        assert(dest.capacity >= src.len);
        assert(dest.allocator == &allocator);
        if (src.buffer && src.len > 0) {
            assert_bytes_match(dest.buffer, src.buffer, src.len);
        }
    } else { // Failure path assertions
        assert(aws_byte_buf_is_valid(&old_dest));
        assert(dest.len == 0);
        assert(dest.buffer == NULL);
        assert(dest.capacity == 0);
        assert(dest.allocator == &allocator);
    }

    // Free allocated memory
    if (src.buffer) {
        free(src.buffer);
    }
    if (dest.buffer) {
        free(dest.buffer);
    }
}
