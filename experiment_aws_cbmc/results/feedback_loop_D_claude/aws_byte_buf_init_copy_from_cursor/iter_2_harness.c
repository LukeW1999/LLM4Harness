#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <string.h>

void aws_byte_buf_init_copy_from_cursor_harness(void) {
    /* Setup: create a non-deterministic aws_byte_cursor */
    struct aws_byte_cursor src;
    src.len = nondet_size_t();
    __CPROVER_assume(aws_byte_cursor_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_cursor_is_valid(&src));

    /* Setup: allocator */
    struct aws_allocator *allocator = aws_default_allocator();

    /* Setup: dest buffer (uninitialized, will be written by function) */
    struct aws_byte_buf dest;

    /* Save old src state (passed by value, but let's verify) */
    struct aws_byte_cursor old_src = src;

    /* Call the function under test */
    int result = aws_byte_buf_init_copy_from_cursor(&dest, allocator, src);

    /* Verify return value is either success or error */
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (result == AWS_OP_SUCCESS) {
        /* SUCCESS PATH assertions */
        
        /* dest->len must equal src.len */
        assert(dest.len == src.len);
        
        /* dest->capacity must equal src.len */
        assert(dest.capacity == src.len);
        
        /* dest->allocator must be the allocator passed in */
        assert(dest.allocator == allocator);
        
        /* If src.len > 0, buffer must be non-NULL */
        if (src.len > 0) {
            assert(dest.buffer != NULL);
            /* Contents must match */
            assert_bytes_match(dest.buffer, src.ptr, src.len);
        } else {
            /* If src.len == 0, buffer must be NULL */
            assert(dest.buffer == NULL);
        }
        
        /* dest must be valid after successful call */
        assert(aws_byte_buf_is_valid(&dest));
        
    } else {
        /* FAILURE PATH assertions */
        /* AWS_ZERO_STRUCT was called, so dest is zeroed */
        /* src.len must have been > 0 (otherwise success would have been returned) */
        assert(src.len != 0);
        
        /* After AWS_ZERO_STRUCT and failed allocation, dest fields are zeroed */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
    }

    /* Frame condition: src is passed by value, original cursor unchanged */
    assert(old_src.len == src.len);
    assert(old_src.ptr == src.ptr);
}
