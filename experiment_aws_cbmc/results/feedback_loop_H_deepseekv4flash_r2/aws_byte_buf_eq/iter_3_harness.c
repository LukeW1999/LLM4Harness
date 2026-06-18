#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_init_copy_harness() {
    /* parameters */
    struct aws_byte_buf dest;
    struct aws_allocator *allocator = aws_default_allocator();
    struct aws_byte_buf src;

    /* precondition: allocator is non-null */
    __CPROVER_assume(allocator != NULL);

    /* precondition: src is a valid byte buf */
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* save old src state for immutability check */
    struct aws_byte_buf old_src = src;
    struct store_byte_from_buffer old_src_bytes;
    if (src.len > 0 && src.buffer != NULL) {
        save_byte_from_array(src.buffer, src.len, &old_src_bytes);
    }

    /* call the function */
    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    /* postcondition: src is unchanged */
    assert(aws_byte_buf_is_valid(&src));
    if (src.len > 0 && src.buffer != NULL) {
        assert_byte_from_buffer_matches(src.buffer, src.len, &old_src_bytes);
    }
    assert(src.allocator == old_src.allocator);
    assert(src.buffer == old_src.buffer);
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);

    if (result == AWS_OP_SUCCESS) {
        /* success */
        if (src.buffer == NULL) {
            /* dest is zeroed, only allocator set */
            assert(dest.buffer == NULL);
            assert(dest.len == 0);
            assert(dest.capacity == 0);
            assert(dest.allocator == allocator);
        } else {
            /* dest is a copy of src with new buffer and allocator */
            assert(dest.buffer != NULL);
            assert(AWS_MEM_IS_WRITABLE(dest.buffer, dest.capacity));
            assert(dest.len == src.len);
            assert(dest.capacity == src.capacity);
            assert(dest.allocator == allocator);
            /* contents match up to src.len */
            if (src.len > 0) {
                assert_bytes_match(dest.buffer, src.buffer, src.len);
            }
        }
    } else {
        /* failure: dest is zeroed */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
    }

    /* validity postcondition */
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_buf_is_valid(&src));
}
