#include <assert.h>
#include <stddef.h>
#include <aws/common/byte_buf.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_byte_buf_init_copy_harness(void) {
    /* allocator must be non‑NULL */
    struct aws_allocator *allocator;
    __CPROVER_assume(allocator != NULL);

    /* source buffer */
    struct aws_byte_buf src;
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* save old source state for immutability checks */
    struct aws_byte_buf old_src = src;
    struct store_byte_from_buffer src_storage;
    if (src.buffer != NULL && src.capacity > 0) {
        save_byte_from_array(src.buffer, src.capacity, &src_storage);
    }

    /* destination buffer (uninitialized) */
    struct aws_byte_buf dest;

    /* call function under test */
    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    /* general invariants */
    assert(aws_byte_buf_is_valid(&dest));

    /* source must remain unchanged */
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.buffer == old_src.buffer);
    assert(src.allocator == old_src.allocator);
    if (src.buffer != NULL && src.capacity > 0) {
        assert_byte_from_buffer_matches(src.buffer, &src_storage);
    }

    if (result == AWS_OP_SUCCESS) {
        /* fields that must be set on success */
        assert(dest.allocator == allocator);
        assert(dest.capacity == src.capacity);
        assert(dest.len == src.len);
        if (src.capacity > 0) {
            assert(dest.buffer != NULL);
            if (src.len > 0) {
                assert_bytes_match(dest.buffer, src.buffer, src.len);
            }
        } else {
            assert(dest.buffer == NULL);
        }
    } else {
        /* on failure the destination is zeroed */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
    }
}
