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
    /* allocator – always use the default allocator */
    struct aws_allocator *allocator = aws_default_allocator();

    /* src buffer – must be valid and bounded */
    struct aws_byte_buf src;
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* dest buffer – allocate the struct, bound it, but its internal buffer is
       uninitialized (the function will set it) */
    struct aws_byte_buf dest;
    __CPROVER_assume(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));
    /* No need to ensure an allocated buffer for dest; it will be (re)allocated
       inside the function. */

    /* Save old state for immutability checks */
    struct aws_byte_buf old_dest = dest;
    struct aws_byte_buf old_src  = src;

    /* Call the function under test */
    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    /* Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* allocator is set to the one passed in */
        assert(dest.allocator == allocator);

        /* src must remain unchanged */
        assert(src.buffer   == old_src.buffer);
        assert(src.len      == old_src.len);
        assert(src.capacity == old_src.capacity);
        assert(src.allocator == old_src.allocator);

        /* dest must be a valid byte buffer */
        assert(aws_byte_buf_is_valid(&dest));

        if (src.buffer == NULL) {
            /* When src has no buffer, dest is zeroed */
            assert(dest.buffer   == NULL);
            assert(dest.len      == 0);
            assert(dest.capacity == 0);
        } else {
            /* When src has a buffer, dest copies length and capacity */
            assert(dest.capacity == src.capacity);
            assert(dest.len      == src.len);
            assert(dest.buffer   != NULL);
            /* The contents of the new buffer must match src's contents */
            assert_bytes_match(dest.buffer, src.buffer, src.len);
        }
    } else {
        /* On failure (allocation error) dest is zeroed */
        assert(dest.buffer   == NULL);
        assert(dest.len      == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);

        /* dest must still satisfy the validity predicate */
        assert(aws_byte_buf_is_valid(&dest));

        /* src must remain unchanged */
        assert(src.buffer   == old_src.buffer);
        assert(src.len      == old_src.len);
        assert(src.capacity == old_src.capacity);
        assert(src.allocator == old_src.allocator);
    }
}
