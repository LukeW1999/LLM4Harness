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
    /* 1. Declare and bound the source buffer */
    struct aws_byte_buf src;
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* Destination buffer (uninitialized) */
    struct aws_byte_buf dest;

    /* Use the default allocator */
    struct aws_allocator *allocator = aws_default_allocator();

    /* 2. Save old state of source (must stay unchanged) */
    struct aws_byte_buf old_src = src;

    /* 3. Call the function under test */
    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    /* 4. Assert source unchanged */
    assert(src.buffer == old_src.buffer);
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.allocator == old_src.allocator);

    /* 5. Post‑condition checks for both success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* allocator of dest must be the one passed in */
        assert(dest.allocator == allocator);

        if (src.buffer == NULL) {
            /* When source has no buffer, dest should be empty and have NULL buffer */
            assert(dest.buffer == NULL);
            assert(dest.len == 0);
            assert(dest.capacity == 0);
        } else {
            /* When source has a buffer, dest must have a newly allocated buffer */
            assert(dest.buffer != NULL);
            assert(dest.len == src.len);
            assert(dest.capacity == src.capacity);
        }

        /* dest must satisfy its validity invariant */
        assert(aws_byte_buf_is_valid(&dest));
    } else {
        /* Failure occurs only when allocation fails; dest is zeroed */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);

        /* Even in the error case the struct must be valid */
        assert(aws_byte_buf_is_valid(&dest));
    }
}
