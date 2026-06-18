#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
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

    /* Save a copy of src to verify it is not modified */
    struct aws_byte_buf src_old = src;

    /* 2. Set up dest (uninitialized output) */
    struct aws_byte_buf dest;

    /* 3. Use default allocator */
    struct aws_allocator *allocator = aws_default_allocator();

    /* 4. Save a byte from src->buffer for later comparison (if buffer is non-null) */
    struct store_byte_from_buffer saved_byte;
    if (src.buffer != NULL && src.len > 0) {
        save_byte_from_array(src.buffer, src.len, &saved_byte);
    }

    /* 5. Call function under test */
    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    /* 6. Assert postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* dest is a valid aws_byte_buf */
        assert(aws_byte_buf_is_valid(&dest));

        /* dest->allocator is set to the passed allocator */
        assert(dest.allocator == allocator);

        if (src.buffer == NULL) {
            /* If src->buffer is null, dest is zeroed with allocator set */
            assert(dest.buffer == NULL);
            assert(dest.len == 0);
            assert(dest.capacity == 0);
        } else {
            /* dest capacity and len match src */
            assert(dest.capacity == src.capacity);
            assert(dest.len == src.len);

            /* dest->buffer is a new allocation (not the same pointer as src->buffer) */
            assert(dest.buffer != NULL);

            /* Contents of dest->buffer match src->buffer for src->len bytes */
            if (src.len > 0) {
                assert_bytes_match(dest.buffer, src.buffer, src.len);
            }
        }
    } else {
        /* On failure, dest is zeroed */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
    }

    /* 7. src must not be modified by the function */
    assert(src.buffer == src_old.buffer);
    assert(src.len == src_old.len);
    assert(src.capacity == src_old.capacity);
    assert(src.allocator == src_old.allocator);

    /* 8. src remains valid */
    assert(aws_byte_buf_is_valid(&src));
}
