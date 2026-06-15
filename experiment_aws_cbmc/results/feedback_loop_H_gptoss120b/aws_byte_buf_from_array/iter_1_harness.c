#include <aws/common/byte_buf.h>
#include <assert.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_byte_buf_init_copy_harness(void) {
    /* 1. Declare and bound data structures */
    struct aws_byte_buf dest;
    struct aws_byte_buf src;
    struct aws_allocator *alloc;

    __CPROVER_assume(alloc != NULL);

    __CPROVER_assume(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_bounded(&src,  MAX_BUFFER_SIZE));

    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* 2. Save old state BEFORE calling */
    struct aws_byte_buf old_dest = dest;
    struct aws_byte_buf old_src  = src;

    /* 3. Call function under test */
    int result = aws_byte_buf_init_copy(&dest, alloc, &src);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* dest must reflect src */
        assert(dest.allocator == alloc);
        assert(dest.capacity == src.capacity);
        assert(dest.len == src.len);

        if (src.buffer == NULL) {
            assert(dest.buffer == NULL);
        } else {
            assert(dest.buffer != NULL);
            /* contents must be identical up to src.len */
            assert_bytes_match(dest.buffer, src.buffer, src.len);
        }
    } else {
        /* on failure dest is zeroed */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
    }

    /* 5. Assert fields that must NOT change regardless of result */
    /* src must remain unchanged */
    assert(src.buffer   == old_src.buffer);
    assert(src.len      == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.allocator== old_src.allocator);

    /* allocator argument is unchanged (it is a pointer, not modified) */
    assert(alloc != NULL);

    /* 6. Assert validity invariant always holds */
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_buf_is_valid(&src));
}
