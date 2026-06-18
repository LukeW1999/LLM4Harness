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
    /* 1. Declare inputs */
    struct aws_byte_buf dest;
    struct aws_allocator *alloc = aws_default_allocator();

    struct aws_byte_cursor src;

    /* 2. Bound scalar inputs */
    src.len = nondet_size_t();
    __CPROVER_assume(src.len <= MAX_BUFFER_SIZE);

    /* 3. Allocate buffer members for src */
    ensure_byte_cursor_has_allocated_buffer_member(&src);

    /* 4. Assume src is a valid cursor */
    __CPROVER_assume(aws_byte_cursor_is_valid(&src));

    /* 5. Save old state of src (to check immutability) */
    struct aws_byte_cursor old_src = src;
    struct store_byte_from_buffer src_storage;
    if (src.len > 0) {
        save_byte_from_array(src.ptr, src.len, &src_storage);
    }

    /* 6. Call function under test */
    int result = aws_byte_buf_init_copy_from_cursor(&dest, alloc, src);

    /* 7. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* dest fields must reflect src */
        assert(dest.len == src.len);
        assert(dest.capacity == src.len);
        assert(dest.allocator == alloc);
        if (src.len == 0) {
            assert(dest.buffer == NULL);
        } else {
            assert(dest.buffer != NULL);
            /* content must be a copy of src */
            assert_bytes_match(dest.buffer, src.ptr, src.len);
        }
    } else {
        /* on failure dest must be zeroed */
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.buffer == NULL);
        assert(dest.allocator == NULL);
    }

    /* src must be unchanged regardless of result */
    assert(src.len == old_src.len);
    assert(src.ptr == old_src.ptr);
    if (src.len > 0) {
        assert_byte_from_buffer_matches(src.ptr, &src_storage);
    }

    /* 8. Validity invariants */
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_cursor_is_valid(&src));
}
