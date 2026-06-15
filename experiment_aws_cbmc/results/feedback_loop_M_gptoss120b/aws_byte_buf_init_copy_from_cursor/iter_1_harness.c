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
    /* 1. Declare and bound input cursor */
    struct aws_byte_cursor src;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_cursor_is_valid(&src));

    /* Save old state of src for immutability checks */
    struct aws_byte_cursor old_src = src;

    /* 2. Use default allocator (non‑deterministic allocator not allowed) */
    struct aws_allocator *allocator = aws_default_allocator();

    /* 3. Destination buffer (output) */
    struct aws_byte_buf dest;

    /* 4. Call function under test */
    int result = aws_byte_buf_init_copy_from_cursor(&dest, allocator, src);

    /* 5. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* Fields that must be set on success */
        assert(dest.allocator == allocator);
        assert(dest.len == src.len);
        assert(dest.capacity == src.len);
        if (src.len > 0) {
            assert(dest.buffer != NULL);
            assert_bytes_match(dest.buffer, src.ptr, src.len);
        } else {
            assert(dest.buffer == NULL);
        }

        /* src must remain unchanged */
        assert(src.len == old_src.len);
        assert(src.ptr == old_src.ptr);
    } else {
        /* On allocation failure the function zeroes dest and sets allocator */
        assert(dest.allocator == allocator);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.buffer == NULL);

        /* src must remain unchanged */
        assert(src.len == old_src.len);
        assert(src.ptr == old_src.ptr);
    }

    /* 6. Validity invariants */
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_cursor_is_valid(&src));
}
