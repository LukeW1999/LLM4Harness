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
    /* allocator – use the default allocator as required by the harness rules */
    struct aws_allocator *alloc = aws_default_allocator();

    /* -----------------------------------------------------------------
     * 1. Declare and bound the source byte buffer (input)
     * ----------------------------------------------------------------- */
    struct aws_byte_buf src;
    /* bound the overall size of the buffer */
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));

    /* nondeterministically choose capacity and length within the bound */
    src.capacity = nondet_size_t();
    __CPROVER_assume(src.capacity <= MAX_BUFFER_SIZE);
    src.len = nondet_size_t();
    __CPROVER_assume(src.len <= src.capacity);

    /* allocate the underlying buffer when capacity > 0 */
    if (src.capacity > 0) {
        ensure_byte_buf_has_allocated_buffer_member(&src);
    } else {
        src.buffer = NULL;
    }

    /* src must be a valid aws_byte_buf */
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* keep a copy of the original src for immutability checks */
    struct aws_byte_buf old_src = src;

    /* -----------------------------------------------------------------
     * 2. Declare the destination byte buffer (output) and save its old state
     * ----------------------------------------------------------------- */
    struct aws_byte_buf dest;
    struct aws_byte_buf old_dest = dest; /* contents are nondet – used only for failure checks */

    /* -----------------------------------------------------------------
     * 3. Call the function under verification
     * ----------------------------------------------------------------- */
    int result = aws_byte_buf_init_copy(&dest, alloc, &src);

    /* -----------------------------------------------------------------
     * 4. Post‑condition checks for both success and error paths
     * ----------------------------------------------------------------- */
    if (result == AWS_OP_SUCCESS) {
        /* src must remain unchanged */
        assert(src.buffer == old_src.buffer);
        assert(src.len == old_src.len);
        assert(src.capacity == old_src.capacity);
        assert(src.allocator == old_src.allocator);

        if (src.buffer == NULL) {
            /* src was an empty buffer – dest should be an empty buffer with the same allocator */
            assert(dest.buffer == NULL);
            assert(dest.len == 0);
            assert(dest.capacity == 0);
            assert(dest.allocator == alloc);
        } else {
            /* src had data – dest must contain a copy of that data */
            assert(dest.buffer != NULL);
            assert(dest.buffer != src.buffer);               /* new allocation */
            assert(dest.len == src.len);
            assert(dest.capacity == src.capacity);
            assert(dest.allocator == alloc);
            /* the copied bytes must be identical */
            assert_bytes_match(dest.buffer, src.buffer, src.len);
        }
    } else {
        /* Allocation failed – dest must be zeroed out */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
    }

    /* -----------------------------------------------------------------
     * 5. Fields that must never change regardless of the result
     * ----------------------------------------------------------------- */
    /* (dest fields are fully specified above for each branch) */

    /* -----------------------------------------------------------------
     * 6. Global invariants – the structures must remain valid
     * ----------------------------------------------------------------- */
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_buf_is_valid(&src));
}
