#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_buf_init_copy_from_cursor_harness()
{
    /* Declare and bound the source cursor */
    struct aws_byte_cursor src;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_cursor_is_valid(&src));

    /* allocator is always valid */
    struct aws_allocator *allocator = aws_default_allocator();

    /* dest is uninitialized, will be fully written by the function */
    struct aws_byte_buf dest;

    /* call the function under test */
    int result = aws_byte_buf_init_copy_from_cursor(&dest, allocator, src);

    /* Postconditions for the SUCCESS path */
    if (result == AWS_OP_SUCCESS)
    {
        assert(dest.len == src.len);
        assert(dest.capacity == src.len);
        assert(dest.allocator == allocator);

        if (src.len > 0)
        {
            /* buffer must be allocated and contain a copy of the source data */
            assert(dest.buffer != NULL);
            assert_bytes_match(dest.buffer, src.ptr, src.len);
        }
        else
        {
            /* zero-length src -> NULL buffer */
            assert(dest.buffer == NULL);
        }
    }
    else
    {
        /* FAILURE path: dest must be zeroed out (as done by AWS_ZERO_STRUCT at start) */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
    }

    /* Validity invariants: dest must always be valid after the call */
    assert(aws_byte_buf_is_valid(&dest));

    /* src must remain untouched (it is a const parameter) */
    assert(aws_byte_cursor_is_valid(&src));
}
