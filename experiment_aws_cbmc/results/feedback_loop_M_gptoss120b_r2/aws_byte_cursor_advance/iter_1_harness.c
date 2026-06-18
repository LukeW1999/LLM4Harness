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
    /* allocator (always use the default allocator) */
    struct aws_allocator *alloc = aws_default_allocator();

    /* src cursor: nondeterministic but bounded */
    struct aws_byte_cursor src;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&src);
    __CPROVER_assume(src.len <= MAX_BUFFER_SIZE);
    __CPROVER_assume(src.ptr != NULL || src.len == 0);
    __CPROVER_assume(aws_byte_cursor_is_valid(&src));

    /* Save old src for immutability checks */
    struct aws_byte_cursor old_src = src;

    /* dest buffer (output) */
    struct aws_byte_buf dest;

    /* Call the function under test */
    int result = aws_byte_buf_init_copy_from_cursor(&dest, alloc, src);

    /* Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* allocator must be set to the provided allocator */
        assert(dest.allocator == alloc);

        /* length and capacity must equal src.len */
        assert(dest.len == src.len);
        assert(dest.capacity == src.len);

        /* buffer allocation semantics */
        if (src.len > 0) {
            assert(dest.buffer != NULL);
            /* the newly allocated buffer must contain a copy of src data */
            assert_bytes_match(dest.buffer, src.ptr, src.len);
        } else {
            assert(dest.buffer == NULL);
        }

        /* src must be unchanged */
        assert(src.ptr == old_src.ptr);
        assert(src.len == old_src.len);
    } else {
        /* on allocation failure the dest struct is zeroed */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);

        /* src must be unchanged */
        assert(src.ptr == old_src.ptr);
        assert(src.len == old_src.len);
    }

    /* Validity invariants must hold after the call */
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_cursor_is_valid(&src));
}
