#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_byte_buf_init_copy_harness() {
    /* 1. Declare and bound data structures */
    struct aws_byte_buf dest;
    struct aws_allocator *allocator = aws_default_allocator();
    struct aws_byte_buf src;
    size_t max_size = MAX_BUFFER_SIZE;
    uint8_t src_data[MAX_BUFFER_SIZE];
    size_t src_len;
    __CPROVER_assume(src_len <= max_size);
    src.buffer = src_data;
    src.len = src_len;
    src.capacity = max_size;
    src.allocator = allocator;
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_byte_buf old_dest = dest;
    struct aws_byte_buf old_src = src;

    /* 3. Call function under test */
    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        assert(dest.len == src.len);
        assert(dest.capacity >= src.len);
        assert(dest.allocator == allocator);
        assert(AWS_MEM_IS_WRITABLE(dest.buffer, dest.capacity));
        assert_bytes_match(dest.buffer, src.buffer, src.len);
    } else {
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == allocator);
        assert(dest.buffer == NULL);
    }

    /* 5. Assert fields that must NOT change regardless of result */
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.allocator == old_src.allocator);
    assert(src.buffer == old_src.buffer);

    /* 6. Assert validity invariant always holds */
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_buf_is_valid(&src));
    if (allocator) {
        assert(aws_allocator_is_valid(allocator));
    }
}
