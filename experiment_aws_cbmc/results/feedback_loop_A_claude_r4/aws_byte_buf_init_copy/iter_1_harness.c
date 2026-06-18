#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_buf_init_copy_harness(void) {
    /* 1. Set up src buffer */
    struct aws_byte_buf src;
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* Save a byte from src buffer for immutability check */
    struct store_byte_from_buffer src_byte;
    if (src.buffer != NULL && src.len > 0) {
        save_byte_from_array(src.buffer, src.len, &src_byte);
    }

    /* Save old src state to verify it's not modified */
    struct aws_byte_buf old_src = src;

    /* 2. Set up dest (uninitialized output) */
    struct aws_byte_buf dest;

    /* 3. Get allocator */
    struct aws_allocator *allocator = aws_default_allocator();

    /* 4. Call function under test */
    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    /* 5. Assert postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* dest is valid */
        assert(aws_byte_buf_is_valid(&dest));

        /* dest->allocator is set to the passed allocator */
        assert(dest.allocator == allocator);

        if (src.buffer == NULL) {
            /* If src buffer is null, dest has null buffer with len=0 and capacity=0 */
            assert(dest.buffer == NULL);
            assert(dest.len == 0);
            assert(dest.capacity == 0);
        } else {
            /* dest capacity and len match src */
            assert(dest.capacity == src.capacity);
            assert(dest.len == src.len);

            /* dest has a new backing array (different pointer) */
            assert(dest.buffer != NULL);

            /* Contents are copied from src */
            if (src.len > 0) {
                assert_bytes_match(dest.buffer, src.buffer, src.len);
            }
        }
    } else {
        /* On failure, dest is zeroed out */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
    }

    /* 6. src must not be modified */
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.buffer == old_src.buffer);
    assert(src.allocator == old_src.allocator);

    /* src remains valid */
    assert(aws_byte_buf_is_valid(&src));

    /* Verify a byte from src buffer was not modified */
    if (old_src.buffer != NULL && old_src.len > 0) {
        assert_byte_from_buffer_matches(src.buffer, &src_byte);
    }
}
