#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

void aws_byte_buf_init_copy_harness() {
    /* 1. Declare and bound data structures */
    struct aws_byte_buf dest;
    struct aws_allocator *allocator = can_fail_allocator();
    struct aws_byte_buf src;
    size_t max_size;
    __CPROVER_assume(max_size <= MAX_BUFFER_SIZE);
    uint8_t *src_buffer = malloc(max_size);
    __CPROVER_assume(src_buffer != NULL);
    src.buffer = src_buffer;
    src.len = max_size;
    src.capacity = max_size;
    src.allocator = allocator;

    /* 2. Initialize destination buffer */
    dest.buffer = NULL;
    dest.len = 0;
    dest.capacity = 0;
    dest.allocator = allocator;

    /* 3. Save old state BEFORE calling — needed for immutability checks */
    struct aws_byte_buf old_dest = dest;
    struct aws_byte_buf old_src = src;

    /* 4. Call function under test */
    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    /* 5. Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        assert(dest.len == src.len);
        assert(dest.capacity >= src.capacity);
        assert(memcmp(dest.buffer, src.buffer, src.len) == 0);
        assert(dest.allocator == allocator);
    } else {
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.buffer == NULL);
        assert(dest.allocator == allocator);
    }

    /* 6. Assert fields that must NOT change regardless of result */
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.buffer == old_src.buffer);
    assert(src.allocator == old_src.allocator);

    /* 7. Assert validity invariant always holds */
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_buf_is_valid(&src));

    /* 8. Free allocated memory */
    free(src_buffer);
}
