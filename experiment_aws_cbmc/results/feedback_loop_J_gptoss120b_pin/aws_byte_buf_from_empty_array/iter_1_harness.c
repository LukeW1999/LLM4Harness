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
    /* allocator – use the default allocator as required by the library */
    struct aws_allocator *allocator = aws_default_allocator();

    /* src buffer – nondeterministically generated and bounded */
    struct aws_byte_buf src;
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* Save a copy of src for immutability checks */
    struct aws_byte_buf old_src = src;

    /* dest buffer – uninitialized; the function will initialize it */
    struct aws_byte_buf dest;

    /* Call the function under test */
    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    /* -------------------------------------------------------------------- */
    /* Success path */
    if (result == AWS_OP_SUCCESS) {
        /* dest must be a valid byte buffer */
        assert(aws_byte_buf_is_valid(&dest));

        /* allocator field is set to the supplied allocator */
        assert(dest.allocator == allocator);

        if (src.buffer == NULL) {
            /* When src has no buffer, dest should be empty and have a NULL buffer */
            assert(dest.buffer == NULL);
            assert(dest.len == 0);
            assert(dest.capacity == 0);
        } else {
            /* When src has a buffer, dest gets its own allocation */
            assert(dest.buffer != NULL);
            assert(dest.buffer != src.buffer);               /* new allocation, not aliasing src */
            assert(dest.len == src.len);
            assert(dest.capacity == src.capacity);
            /* The contents must be identical to src's contents */
            assert_bytes_match(dest.buffer, src.buffer, src.len);
        }

        /* src must remain unchanged */
        assert(src.buffer == old_src.buffer);
        assert(src.len == old_src.len);
        assert(src.capacity == old_src.capacity);
        assert(src.allocator == old_src.allocator);
    }
    /* -------------------------------------------------------------------- */
    /* Failure path */
    else {
        /* The function returns AWS_OP_ERR only when allocation fails and src.buffer != NULL */
        assert(result == AWS_OP_ERR);

        /* dest is zeroed out on allocation failure */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);

        /* dest must still satisfy the validity predicate (empty buffer is valid) */
        assert(aws_byte_buf_is_valid(&dest));

        /* src must remain unchanged */
        assert(src.buffer == old_src.buffer);
        assert(src.len == old_src.len);
        assert(src.capacity == old_src.capacity);
        assert(src.allocator == old_src.allocator);
    }
}
