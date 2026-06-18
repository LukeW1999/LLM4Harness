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
    /* 1. Declare inputs and bound them */
    struct aws_byte_cursor src;
    /* nondet length bounded by MAX_BUFFER_SIZE */
    src.len = nondet_size_t();
    __CPROVER_assume(src.len <= MAX_BUFFER_SIZE);
    /* allocate buffer for src if length > 0 */
    ensure_byte_cursor_has_allocated_buffer_member(&src);
    /* ensure src is a valid cursor */
    __CPROVER_assume(aws_byte_cursor_is_valid(&src));

    /* 2. Destination buffer (output) */
    struct aws_byte_buf dest;

    /* 3. Save old state of src for immutability checks */
    struct aws_byte_cursor old_src = src;

    /* 4. Use the default allocator */
    struct aws_allocator *alloc = aws_default_allocator();

    /* 5. Call function under test */
    int result = aws_byte_buf_init_copy_from_cursor(&dest, alloc, src);

    /* 6. Postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* dest must reflect src */
        assert(dest.allocator == alloc);
        assert(dest.len == src.len);
        assert(dest.capacity == src.len);
        if (src.len > 0) {
            assert(dest.buffer != NULL);
            assert_bytes_match(dest.buffer, src.ptr, src.len);
        } else {
            /* zero-length source yields NULL buffer */
            assert(dest.buffer == NULL);
        }
    } else {
        /* on failure dest should be zeroed */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
    }

    /* src must be unchanged regardless of result */
    assert(src.len == old_src.len);
    assert(src.ptr == old_src.ptr);
    if (src.len > 0) {
        assert_bytes_match(src.ptr, old_src.ptr, src.len);
    }

    /* 7. Validity invariants */
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_cursor_is_valid(&src));
}
