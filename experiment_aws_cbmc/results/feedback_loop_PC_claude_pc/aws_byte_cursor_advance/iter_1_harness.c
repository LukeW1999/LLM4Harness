#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_init_copy_harness(void) {
    /* 1. Set up dest (output parameter - just needs to be a valid pointer) */
    struct aws_byte_buf dest;

    /* 2. Set up allocator */
    struct aws_allocator *allocator = aws_default_allocator();
    __CPROVER_assume(allocator != NULL);

    /* 3. Set up src (input - must be a valid aws_byte_buf) */
    struct aws_byte_buf src;
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* 4. Save src state before call for immutability checks */
    struct aws_byte_buf src_old = src;
    /* Save a byte from src buffer for content immutability check */
    struct store_byte_from_buffer src_byte_storage;
    if (src.buffer != NULL && src.len > 0) {
        save_byte_from_array(src.buffer, src.len, &src_byte_storage);
    }

    /* 5. Call function under test */
    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    /* 6. Assert postconditions */

    /* --- Success path --- */
    if (result == AWS_OP_SUCCESS) {
        /* dest is valid */
        assert(aws_byte_buf_is_valid(&dest));

        /* allocator of dest is the passed allocator */
        assert(dest.allocator == allocator);

        if (src.buffer == NULL) {
            /* If src buffer is null, dest has null buffer with len=0 and capacity=0 */
            assert(dest.buffer == NULL);
            assert(dest.len == 0);
            assert(dest.capacity == 0);
        } else {
            /* dest capacity equals src capacity */
            assert(dest.capacity == src.capacity);
            /* dest len equals src len */
            assert(dest.len == src.len);
            /* dest buffer is not null (new allocation) */
            assert(dest.buffer != NULL);
            /* dest buffer is different from src buffer (new allocation) */
            assert(dest.buffer != src.buffer);
            /* Contents match: bytes were copied */
            if (src.len > 0) {
                assert_bytes_match(dest.buffer, src.buffer, src.len);
            }
        }
    } else {
        /* --- Failure path --- */
        /* On failure, dest is zeroed out */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
    }

    /* 7. src must not have been modified (frame condition) */
    assert(src.buffer == src_old.buffer);
    assert(src.len == src_old.len);
    assert(src.capacity == src_old.capacity);
    assert(src.allocator == src_old.allocator);

    /* Verify src byte contents unchanged */
    if (src.buffer != NULL && src.len > 0) {
        assert_byte_from_buffer_matches(src.buffer, &src_byte_storage);
    }

    /* 8. src remains valid */
    assert(aws_byte_buf_is_valid(&src));
}
