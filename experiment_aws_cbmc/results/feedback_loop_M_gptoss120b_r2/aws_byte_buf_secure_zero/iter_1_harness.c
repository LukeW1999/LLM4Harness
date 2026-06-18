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
    /* 1. Declare data structures */
    struct aws_byte_buf dest;
    struct aws_byte_cursor src;
    struct aws_allocator *alloc = aws_default_allocator();

    /* 2. Allocate and bound src cursor */
    ensure_byte_cursor_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_cursor_is_bounded(&src, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_cursor_is_valid(&src));

    /* 3. Save old state of src (dest will be overwritten) */
    struct aws_byte_cursor old_src = src;

    /* 4. Call function under test */
    int result = aws_byte_buf_init_copy_from_cursor(&dest, alloc, src);

    /* 5. General post‑conditions */
    /* dest must always be a valid byte buffer */
    assert(aws_byte_buf_is_valid(&dest));

    /* src must remain unchanged */
    assert(src.ptr == old_src.ptr);
    assert(src.len == old_src.len);

    if (result == AWS_OP_SUCCESS) {
        /* Success path: dest reflects src */
        assert(dest.len == src.len);
        assert(dest.capacity == src.len);
        assert(dest.allocator == alloc);
        if (src.len > 0) {
            assert(dest.buffer != NULL);
            /* The contents must be a copy of src */
            assert_bytes_match(dest.buffer, src.ptr, src.len);
        } else {
            /* Zero‑length src yields a NULL buffer */
            assert(dest.buffer == NULL);
        }
    } else {
        /* Failure path: allocation failed, dest should be zeroed */
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.buffer == NULL);
        assert(dest.allocator == NULL);
    }
}
