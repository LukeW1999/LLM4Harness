#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

#define MAX_BUFFER_SIZE 256

static void assert_bytes_match(const uint8_t *a, const uint8_t *b, size_t len) {
    for (size_t i = 0; i < len; ++i) {
        assert(a[i] == b[i]);
    }
}

void aws_byte_buf_init_copy_from_cursor_harness(void) {
    /* 1. Declare and bound source cursor */
    struct aws_byte_cursor src;
    src.len = nondet_size_t();
    __CPROVER_assume(src.len <= MAX_BUFFER_SIZE);
    ensure_byte_cursor_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_cursor_is_valid(&src));

    /* 2. Destination buffer (output) */
    struct aws_byte_buf dest = {0};

    /* 3. Save old state of src for immutability checks */
    struct aws_byte_cursor old_src = src;

    /* 4. Use the default allocator */
    struct aws_allocator *alloc = aws_default_allocator();

    /* 5. Call function under test */
    int result = aws_byte_buf_init_copy_from_cursor(&dest, alloc, src);

    /* 6. Postconditions */
    if (result == AWS_OP_SUCCESS) {
        assert(dest.allocator == alloc);
        assert(dest.len == src.len);
        assert(dest.capacity == src.len);
        if (src.len > 0) {
            assert(dest.buffer != NULL);
            assert_bytes_match(dest.buffer, src.ptr, src.len);
        } else {
            assert(dest.buffer == NULL);
        }
    } else {
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
