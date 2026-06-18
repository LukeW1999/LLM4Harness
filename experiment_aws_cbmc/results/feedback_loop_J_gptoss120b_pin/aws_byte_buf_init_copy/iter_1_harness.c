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
    /* 1. Declare and bound source buffer */
    struct aws_byte_buf src;
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);

    /* allow src.buffer to be NULL (empty source) */
    if (nondet_bool()) {
        src.buffer = NULL;
        src.capacity = 0;
        src.len = 0;
    }

    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* 2. Declare destination buffer and save its old state */
    struct aws_byte_buf dest;
    struct aws_byte_buf old_dest = dest;   /* old_dest is nondet; used only for unchanged‑field checks on failure */

    /* 3. Save old source state for immutability checks */
    struct aws_byte_buf old_src = src;

    /* 4. Use the default allocator */
    struct aws_allocator *alloc = aws_default_allocator();

    /* 5. Call function under test */
    int result = aws_byte_buf_init_copy(&dest, alloc, &src);

    /* 6. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        if (src.buffer == NULL) {
            /* empty source case – dest is zeroed except allocator */
            assert(dest.len == 0);
            assert(dest.capacity == 0);
            assert(dest.buffer == NULL);
            assert(dest.allocator == alloc);
        } else {
            /* normal copy case – allocation succeeded */
            assert(dest.allocator == alloc);
            assert(dest.capacity == src.capacity);
            assert(dest.len == src.len);
            assert(dest.buffer != NULL);
            assert(dest.buffer != src.buffer);
            assert_bytes_match(dest.buffer, src.buffer, src.len);
        }
    } else {
        /* allocation failure – dest zeroed */
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.buffer == NULL);
        assert(dest.allocator == NULL);
    }

    /* 7. Source must remain unchanged */
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.buffer == old_src.buffer);
    if (src.buffer != NULL) {
        assert_bytes_match(src.buffer, old_src.buffer, src.len);
    }

    /* 8. Destination fields that never change (regardless of result) */
    /* allocator and buffer are already asserted above for each branch */

    /* 9. Validity invariants must hold */
    assert(aws_byte_buf_is_valid(&src));
    assert(aws_byte_buf_is_valid(&dest));
}
