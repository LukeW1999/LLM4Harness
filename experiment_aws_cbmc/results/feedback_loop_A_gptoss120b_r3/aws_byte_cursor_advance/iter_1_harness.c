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

void aws_byte_buf_init_copy_harness() {
    /* 1. Declare data structures */
    struct aws_byte_buf dest;
    struct aws_byte_buf src;
    struct aws_allocator *allocator = aws_default_allocator();

    /* bound src and allocate its buffer */
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* bound dest (its current contents are irrelevant, but we allocate a buffer
       to keep the model well‑formed) */
    __CPROVER_assume(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&dest);
    /* dest does NOT need to be valid before the call – only non‑NULL */

    /* 2. Save old state */
    struct aws_byte_buf old_dest = dest;
    struct aws_byte_buf old_src   = src;

    /* 3. Call function under test */
    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    /* 4. Post‑condition checks for both success and failure */
    if (result == AWS_OP_SUCCESS) {
        /* dest must be a valid buffer */
        assert(aws_byte_buf_is_valid(&dest));

        /* allocator field is set to the supplied allocator */
        assert(dest.allocator == allocator);

        if (src.buffer == NULL) {
            /* When source has no buffer, dest is zeroed */
            assert(dest.buffer == NULL);
            assert(dest.len == 0);
            assert(dest.capacity == 0);
        } else {
            /* When source has a buffer, dest mirrors length and capacity */
            assert(dest.buffer != NULL);
            assert(dest.len == src.len);
            assert(dest.capacity == src.capacity);

            /* The new buffer must be distinct from the source buffer */
            assert(dest.buffer != src.buffer);

            /* Contents of the new buffer must equal the source contents */
            assert_bytes_match(dest.buffer, src.buffer, src.len);
        }
    } else {
        /* On allocation failure the destination is zeroed */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);

        /* Zeroed buffer is still considered valid */
        assert(aws_byte_buf_is_valid(&dest));

        /* Source must remain unchanged */
        assert(src.buffer   == old_src.buffer);
        assert(src.len      == old_src.len);
        assert(src.capacity == old_src.capacity);
        assert(src.allocator== old_src.allocator);
    }

    /* 5. Fields that never change regardless of outcome */
    /* (none for dest – all fields are either set or zeroed) */

    /* 6. Global invariants */
    assert(aws_byte_buf_is_valid(&src));
}
