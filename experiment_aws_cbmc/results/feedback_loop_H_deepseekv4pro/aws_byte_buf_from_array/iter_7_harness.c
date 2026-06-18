#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 1000
#endif

void aws_byte_buf_init_copy_from_cursor_harness() {
    /* 1. Declare and initialize */
    struct aws_allocator *allocator = aws_default_allocator();
    assert(allocator != NULL);

    struct aws_byte_buf dest;
    struct aws_byte_cursor src;

    /* 2. Bound and allocate src cursor */
    __CPROVER_assume(aws_byte_cursor_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_cursor_is_valid(&src));

    /* 3. Save old state of src */
    struct aws_byte_cursor old_src = src;

    /* 4. Call function under test */
    int result = aws_byte_buf_init_copy_from_cursor(&dest, allocator, src);

    /* 5. Global postconditions */
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_cursor_is_valid(&src));

    /* 6. src immutability (passed by value but we check) */
    assert(src.ptr == old_src.ptr);
    assert(src.len == old_src.len);

    /* 7. Branch on result */
    if (result == AWS_OP_SUCCESS) {
        /* Success: len and capacity equal src.len, allocator set */
        assert(dest.len == src.len);
        assert(dest.capacity == src.len);
        assert(dest.allocator == allocator);

        if (src.len == 0) {
            assert(dest.buffer == NULL);
        } else {
            assert(dest.buffer != NULL);
            assert(memcmp(dest.buffer, src.ptr, src.len) == 0);
        }
    } else {
        /* Failure: allocation failed for non‑zero src */
        assert(result == AWS_OP_ERR);
        /* After AWS_ZERO_STRUCT all fields are zero */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
    }
}
