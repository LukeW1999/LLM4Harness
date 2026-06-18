#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <string.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_init_copy_harness() {
    /* 1. Declare and bound source buffer (valid) */
    struct aws_byte_buf src;
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* Save old src for immutability checks */
    struct aws_byte_buf old_src = src;

    /* 2. Destination buffer (uninitialized) and default allocator */
    struct aws_byte_buf dest;
    struct aws_allocator *allocator = aws_default_allocator();

    /* 3. Call function under test */
    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    /* 4. Assert validity invariants */
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_buf_is_valid(&src));

    /* 5. Immutability of source */
    assert(src.allocator == old_src.allocator);
    assert(src.capacity == old_src.capacity);
    assert(src.len == old_src.len);
    assert(src.buffer == old_src.buffer);
    if (src.len > 0 && src.buffer != NULL) {
        assert(memcmp(src.buffer, old_src.buffer, src.len) == 0);
    }

    /* 6. Postconditions for BOTH success and failure */
    if (result == AWS_OP_SUCCESS) {
        /* Success: dest is a deep copy of src */
        assert(dest.allocator == allocator);
        assert(dest.len == src.len);
        assert(dest.capacity == src.capacity);
        if (src.buffer == NULL) {
            assert(dest.buffer == NULL);
        } else {
            assert(dest.buffer != NULL);
            assert(memcmp(dest.buffer, src.buffer, src.len) == 0);
        }
    } else {
        /* Failure: memory allocation failed */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        /* AWS_ZERO_STRUCT clears allocator as well */
        assert(dest.allocator == NULL);
    }
}
