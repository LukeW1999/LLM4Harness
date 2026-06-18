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
    /* src buffer – nondeterministic but bounded and valid */
    struct aws_byte_buf src;
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* keep a copy of src for immutability checks */
    struct aws_byte_buf old_src = src;
    struct store_byte_from_buffer src_storage;
    if (src.buffer != NULL) {
        save_byte_from_array(src.buffer, src.capacity, &src_storage);
    }

    /* dest buffer – uninitialized, will be overwritten by the function */
    struct aws_byte_buf dest;
    struct aws_byte_buf old_dest = dest; /* capture old state (may be nondet) */

    /* allocator – always use the default allocator */
    struct aws_allocator *alloc = aws_default_allocator();

    /* call the function under test */
    int result = aws_byte_buf_init_copy(&dest, alloc, &src);

    /* src must remain unchanged regardless of the outcome */
    assert(src.buffer == old_src.buffer);
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.allocator == old_src.allocator);
    if (src.buffer != NULL) {
        assert_byte_from_buffer_matches(src.buffer, &src_storage);
    }

    /* dest must always satisfy its validity invariant */
    assert(aws_byte_buf_is_valid(&dest));

    if (result == AWS_OP_SUCCESS) {
        /* allocator field is set to the supplied allocator */
        assert(dest.allocator == alloc);

        if (src.buffer == NULL) {
            /* Special case: src has no buffer – dest is a zero‑length buffer */
            assert(dest.buffer == NULL);
            assert(dest.len == 0);
            assert(dest.capacity == 0);
        } else {
            /* Normal case: allocation succeeded */
            assert(dest.buffer != NULL);
            assert(dest.len == src.len);
            assert(dest.capacity == src.capacity);
            /* The contents up to src.len must be a copy of src's contents */
            assert_bytes_match(dest.buffer, src.buffer, src.len);
        }
    } else {
        /* On failure the function zeroes the destination structure */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
    }
}
