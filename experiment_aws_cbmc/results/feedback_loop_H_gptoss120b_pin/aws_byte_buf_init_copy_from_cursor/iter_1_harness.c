#include <aws/common/byte_buf.h>
#include <aws/common/array_list.h>
#include <aws/common/linked_list.h>
#include <aws/common/math.h>
#include <aws/common/string.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_init_copy_from_cursor_harness(void) {
    /* 1. Declare and bound data structures */
    struct aws_byte_buf dest;
    struct aws_byte_cursor src;
    struct aws_allocator *allocator = aws_default_allocator();

    __CPROVER_assume(aws_byte_cursor_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_cursor_is_valid(&src));

    /* Save old state of src (dest will be overwritten) */
    struct aws_byte_cursor old_src = src;

    /* 3. Call function under test */
    int result = aws_byte_buf_init_copy_from_cursor(&dest, allocator, src);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* Fields that must be set on success */
        assert(dest.len == src.len);
        assert(dest.capacity == src.len);
        assert(dest.allocator == allocator);
        assert(dest.buffer != NULL);
        /* The allocated buffer must be writable for src.len bytes */
        __CPROVER_assert(AWS_MEM_IS_WRITABLE(dest.buffer, src.len),
                         "dest.buffer must be writable for src.len bytes");
        /* Contents must match the source cursor */
        assert_bytes_match(dest.buffer, src.ptr, src.len);
        /* Validity invariant */
        assert(aws_byte_buf_is_valid(&dest));
    } else {
        /* Failure occurs only when allocation fails (src.len > 0) */
        assert(dest.buffer == NULL);
        assert(dest.len == src.len);
        assert(dest.capacity == src.len);
        assert(dest.allocator == allocator);
        /* No guarantee of validity on failure; we only check that src is unchanged */
        assert(src.len == old_src.len);
        assert(src.ptr == old_src.ptr);
    }

    /* 5. Assert fields that must NOT change regardless of result */
    /* src must remain unchanged */
    assert(src.len == old_src.len);
    assert(src.ptr == old_src.ptr);
    /* src validity must still hold */
    assert(aws_byte_cursor_is_valid(&src));
}
