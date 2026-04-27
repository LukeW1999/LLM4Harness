#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_init_copy_harness() {
    struct aws_byte_buf dest;
    struct aws_byte_buf src;
    struct aws_allocator *allocator = aws_default_allocator();

    // Initialize src with bounded values
    size_t max_src_capacity = MAX_BUFFER_SIZE;
    src.buffer = bounded_malloc(max_src_capacity);
    src.len = nondet_size_t();
    src.capacity = max_src_capacity;
    src.allocator = allocator;

    // Ensure src is valid
    assume(aws_byte_buf_is_valid(&src));

    // Call the function under test
    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    // Step 1: Success Path
    if (result == AWS_OP_SUCCESS) {
        assert(dest.buffer != NULL);
        assert(dest.len == src.len);
        assert(dest.capacity == src.capacity);
        assert(dest.allocator == allocator);
        assert(aws_byte_buf_is_valid(&dest));
    }
    // Step 2: Failure Path
    else {
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == allocator);
        assert(aws_byte_buf_is_valid(&dest));
    }

    // Step 3: Frame Conditions
    assert(src.buffer != NULL);
    assert(src.len <= src.capacity);
    assert(src.allocator == allocator);
    assert(aws_byte_buf_is_valid(&src));

    // Step 4: Validity Invariants
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_buf_is_valid(&src));

    // Clean up
    if (dest.buffer) {
        aws_mem_release(allocator, dest.buffer);
    }
    if (src.buffer) {
        aws_mem_release(allocator, src.buffer);
    }
}
