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

void aws_byte_buf_init_copy_from_cursor_harness() {
    /* Allocate and bound source cursor */
    struct aws_byte_cursor src;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_cursor_is_valid(&src));

    /* Save old source state for immutability checks */
    struct aws_byte_cursor old_src = src;
    struct store_byte_from_buffer src_storage;
    save_byte_from_array(src.ptr, src.len, &src_storage);

    /* Use the default allocator (deterministic) */
    struct aws_allocator *allocator = aws_default_allocator();

    /* Destination buffer (output) */
    struct aws_byte_buf dest;

    /* Call function under test */
    int result = aws_byte_buf_init_copy_from_cursor(&dest, allocator, src);

    if (result == AWS_OP_SUCCESS) {
        /* Success postconditions */
        assert(dest.len == src.len);
        assert(dest.capacity == src.len);
        assert(dest.allocator == allocator);
        if (src.len > 0) {
            assert(dest.buffer != NULL);
            assert_bytes_match(dest.buffer, src.ptr, src.len);
        } else {
            assert(dest.buffer == NULL);
        }

        /* Source must remain unchanged */
        assert(src.len == old_src.len);
        assert(src.ptr == old_src.ptr);
        assert_byte_from_buffer_matches(src.ptr, &src_storage);
        assert(aws_byte_cursor_is_valid(&src));

        /* Destination must be a valid byte buffer */
        assert(aws_byte_buf_is_valid(&dest));
    } else {
        /* Failure postconditions (allocation failure) */
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.buffer == NULL);
        assert(dest.allocator == NULL);

        /* Source must remain unchanged */
        assert(src.len == old_src.len);
        assert(src.ptr == old_src.ptr);
        assert_byte_from_buffer_matches(src.ptr, &src_storage);
        assert(aws_byte_cursor_is_valid(&src));

        /* Destination (zeroed) must still satisfy validity invariant */
        assert(aws_byte_buf_is_valid(&dest));
    }
}
