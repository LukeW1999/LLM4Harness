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

void aws_byte_buf_init_copy_harness(void) {
    /* 1. Declare and bound data structures */
    struct aws_byte_buf src;
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    struct aws_byte_buf dest;
    /* dest will be overwritten by the function, but bound it to keep CBMC happy */
    __CPROVER_assume(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));

    /* Save old state of src (dest will be overwritten) */
    struct aws_byte_buf old_src = src;

    /* 2. Call function under test */
    int result = aws_byte_buf_init_copy(&dest, aws_default_allocator(), &src);

    /* 3. Postconditions for success path */
    if (result == AWS_OP_SUCCESS) {
        /* allocator must be set to the one we passed */
        assert(dest.allocator == aws_default_allocator());

        if (src.buffer == NULL) {
            /* When src has no buffer, dest should be zeroed except allocator */
            assert(dest.buffer == NULL);
            assert(dest.len == 0);
            assert(dest.capacity == 0);
        } else {
            /* dest must have a non‑NULL buffer */
            assert(dest.buffer != NULL);
            /* len and capacity must match src */
            assert(dest.len == src.len);
            assert(dest.capacity == src.capacity);
            /* contents must be copied */
            assert_bytes_match(dest.buffer, src.buffer, src.len);
        }

        /* dest must be a valid byte buffer */
        assert(aws_byte_buf_is_valid(&dest));
    } else {
        /* 4. Postconditions for failure path */
        /* On allocation failure the function zeroes dest */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);

        /* dest must still be a valid byte buffer (zeroed state is valid) */
        assert(aws_byte_buf_is_valid(&dest));
    }

    /* 5. src must remain unchanged regardless of result */
    assert(src.buffer == old_src.buffer);
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.allocator == old_src.allocator);
    assert(aws_byte_buf_is_valid(&src));
}
