#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_buf_init_copy_harness(void) {
    /* 1. Set up dest (output parameter - just needs to be a valid pointer) */
    struct aws_byte_buf dest;

    /* 2. Set up allocator */
    struct aws_allocator *allocator = aws_default_allocator();

    /* 3. Set up src - must be a valid aws_byte_buf */
    struct aws_byte_buf src;
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* 4. Save old src state to check immutability */
    struct aws_byte_buf old_src = src;

    /* 5. Call function under test */
    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    /* 6. Assert postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* dest must be valid */
        assert(aws_byte_buf_is_valid(&dest));

        /* dest->allocator must equal the allocator parameter */
        assert(dest.allocator == allocator);

        if (src.buffer == NULL) {
            /* If src buffer is null, dest has null buffer with len=0 and capacity=0 */
            assert(dest.buffer == NULL);
            assert(dest.len == 0);
            assert(dest.capacity == 0);
        } else {
            /* dest capacity and len will be equal to src len */
            assert(dest.len == src.len);
            assert(dest.capacity == src.capacity);
            /* dest buffer is a new allocation (not the same pointer as src) */
            assert(dest.buffer != NULL);
            /* Contents should match */
            if (src.len > 0) {
                assert_bytes_match(dest.buffer, src.buffer, src.len);
            }
        }
    } else {
        /* On failure, dest should be zeroed out */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
    }

    /* 7. src must not have been modified */
    assert(src.buffer == old_src.buffer);
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.allocator == old_src.allocator);

    /* 8. src validity invariant must still hold */
    assert(aws_byte_buf_is_valid(&src));
}
