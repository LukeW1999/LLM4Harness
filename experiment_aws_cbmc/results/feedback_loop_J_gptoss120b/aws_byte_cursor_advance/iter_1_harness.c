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
    /* allocator – use the default allocator as required by the harness policy */
    struct aws_allocator *alloc = aws_default_allocator();

    /* 1. Declare and bound the source and destination byte buffers */
    struct aws_byte_buf src;
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    struct aws_byte_buf dest;
    __CPROVER_assume(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&dest);
    __CPROVER_assume(aws_byte_buf_is_valid(&dest));

    /* 2. Save old state for immutability checks */
    struct aws_byte_buf old_dest = dest;
    struct aws_byte_buf old_src  = src;

    /* Save a byte from the source buffer for later content comparison (if any) */
    struct store_byte_from_buffer src_storage;
    if (src.buffer != NULL && src.len > 0) {
        save_byte_from_array(src.buffer, src.len, &src_storage);
    }

    /* 3. Call the function under test */
    int result = aws_byte_buf_init_copy(&dest, alloc, &src);

    /* 4. Post‑condition checks for both success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* Allocator must be the one supplied */
        assert(dest.allocator == alloc);

        /* Length and capacity must mirror the source */
        assert(dest.len == src.len);
        assert(dest.capacity == src.capacity);

        if (src.buffer != NULL) {
            /* Destination must have a non‑NULL buffer */
            assert(dest.buffer != NULL);
            /* The contents of the new buffer must equal the source contents */
            assert_bytes_match(src.buffer, dest.buffer, src.len);
        } else {
            /* When source has no buffer, destination must also have none */
            assert(dest.buffer == NULL);
        }
    } else {
        /* On allocation failure the destination is zeroed out */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
    }

    /* 5. Fields of the source buffer must remain unchanged */
    assert(src.buffer == old_src.buffer);
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.allocator == old_src.allocator);

    /* 6. Destination fields that are not explicitly modified must retain their old values */
    /* (All fields of dest are either set on success or zeroed on failure, so no extra unchanged‑field asserts are needed) */

    /* 7. Validity invariants must hold after the call */
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_buf_is_valid(&src));
}
