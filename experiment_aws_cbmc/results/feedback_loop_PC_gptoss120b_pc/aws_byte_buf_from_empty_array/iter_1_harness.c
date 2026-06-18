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
    /* allocator – use the default allocator (never NULL) */
    struct aws_allocator *alloc = aws_default_allocator();

    /* -----------------------------------------------------------------
     * 1. Declare and bound the source buffer (input)
     * ----------------------------------------------------------------- */
    struct aws_byte_buf src;
    /* nondeterministically choose a capacity within the global bound */
    src.capacity = nondet_size_t();
    __CPROVER_assume(src.capacity <= MAX_BUFFER_SIZE);
    /* choose a length that does not exceed capacity */
    src.len = nondet_size_t();
    __CPROVER_assume(src.len <= src.capacity);
    /* allocator field of src is irrelevant for validity, set to NULL */
    src.allocator = NULL;
    /* allocate the buffer member according to the chosen capacity */
    ensure_byte_buf_has_allocated_buffer_member(&src);
    /* enforce the validity predicate */
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* -----------------------------------------------------------------
     * 2. Save old state of src (must remain unchanged)
     * ----------------------------------------------------------------- */
    struct aws_byte_buf old_src = src;

    /* -----------------------------------------------------------------
     * 3. Destination buffer (output) – uninitialized before the call
     * ----------------------------------------------------------------- */
    struct aws_byte_buf dest;

    /* -----------------------------------------------------------------
     * 4. Call the function under test
     * ----------------------------------------------------------------- */
    int result = aws_byte_buf_init_copy(&dest, alloc, &src);

    /* -----------------------------------------------------------------
     * 5. Post‑condition checks
     * ----------------------------------------------------------------- */
    if (result == AWS_OP_SUCCESS) {
        /* allocator must be set to the one we passed */
        assert(dest.allocator == alloc);

        /* length and capacity must mirror the source */
        assert(dest.len == src.len);
        assert(dest.capacity == src.capacity);

        if (src.buffer == NULL) {
            /* source had no buffer → destination must also have no buffer */
            assert(dest.buffer == NULL);
        } else {
            /* a new buffer must have been allocated */
            assert(dest.buffer != NULL);
            /* contents must be an exact copy of the source */
            assert_bytes_match(dest.buffer, src.buffer, src.len);
        }

        /* validity invariant must hold */
        assert(aws_byte_buf_is_valid(&dest));
    } else {
        /* On allocation failure the destination is zeroed */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);

        /* validity invariant must still hold (zeroed buffer is valid) */
        assert(aws_byte_buf_is_valid(&dest));
    }

    /* -----------------------------------------------------------------
     * 6. Source buffer must be unchanged regardless of the outcome
     * ----------------------------------------------------------------- */
    assert(src.buffer == old_src.buffer);
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.allocator == old_src.allocator);
    /* if there is data, it must be identical */
    if (src.buffer != NULL && src.len > 0) {
        assert_bytes_match(src.buffer, old_src.buffer, src.len);
    }

    /* -----------------------------------------------------------------
     * 7. Global invariants
     * ----------------------------------------------------------------- */
    assert(aws_byte_buf_is_valid(&src));
}
