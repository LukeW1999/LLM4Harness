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
    /* 1. Declare inputs and bound them */
    struct aws_byte_buf dest;
    struct aws_allocator *alloc = aws_default_allocator();
    struct aws_byte_cursor src;

    /* bound dest (output) */
    __CPROVER_assume(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));

    /* bound src (input) */
    __CPROVER_assume(aws_byte_cursor_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_cursor_is_valid(&src));

    /* 2. Save old state for immutability checks */
    struct aws_byte_buf old_dest = dest;
    struct aws_byte_cursor old_src = src;

    struct store_byte_from_buffer src_storage;
    save_byte_from_array(src.ptr, src.len, &src_storage);

    /* 3. Call function under test */
    int result = aws_byte_buf_init_copy_from_cursor(&dest, alloc, src);

    /* 4. Post‑condition checks for both success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* allocator is set to the provided allocator */
        assert(dest.allocator == alloc);

        /* capacity and len equal to src.len */
        assert(dest.capacity == src.len);
        assert(dest.len == src.len);

        if (src.len > 0) {
            /* buffer must be non‑NULL and contain a copy of src */
            assert(dest.buffer != NULL);
            assert_bytes_match(dest.buffer, src.ptr, src.len);
        } else {
            /* zero‑length source yields a NULL buffer */
            assert(dest.buffer == NULL);
        }
    } else {
        /* on failure the dest fields must be zeroed */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == alloc);
    }

    /* 5. Fields that must remain unchanged regardless of result */
    /* src must be unchanged */
    assert(src.ptr == old_src.ptr);
    assert(src.len == old_src.len);
    assert_byte_from_buffer_matches(src.ptr, &src_storage);

    /* dest's allocator field is set (or remains) as above; other fields are covered */

    /* 6. Validity invariants */
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_cursor_is_valid(&src));
}
