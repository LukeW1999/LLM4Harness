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

void aws_byte_buf_init_copy_from_cursor_harness(void) {
    /* allocator – use the default allocator as required */
    struct aws_allocator *allocator = aws_default_allocator();

    /* src cursor – nondeterministic but bounded and valid */
    struct aws_byte_cursor src;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_cursor_is_valid(&src));

    /* Save old state of src for immutability checks */
    struct aws_byte_cursor old_src = src;
    struct store_byte_from_buffer src_storage;
    if (src.len > 0) {
        save_byte_from_array(src.ptr, src.len, &src_storage);
    }

    /* dest buffer – will be initialized by the function */
    struct aws_byte_buf dest;

    /* Call the function under test */
    int result = aws_byte_buf_init_copy_from_cursor(&dest, allocator, src);

    /* src must remain unchanged regardless of the outcome */
    assert(src.len == old_src.len);
    assert(src.ptr == old_src.ptr);
    if (src.len > 0) {
        assert_bytes_match(src.ptr, old_src.ptr, src.len);
        assert_byte_from_buffer_matches(src.ptr, &src_storage);
    }

    if (result == AWS_OP_SUCCESS) {
        /* allocator is propagated */
        assert(dest.allocator == allocator);

        /* length and capacity must equal the source length */
        assert(dest.len == src.len);
        assert(dest.capacity == src.len);

        if (src.len > 0) {
            /* buffer must be allocated and contain a copy of the source data */
            assert(dest.buffer != NULL);
            assert_bytes_match(dest.buffer, src.ptr, src.len);
        } else {
            /* zero‑length source results in a NULL buffer */
            assert(dest.buffer == NULL);
        }

        /* The resulting buffer must be a valid aws_byte_buf */
        assert(aws_byte_buf_is_valid(&dest));
    } else {
        /* Failure can only occur when allocation of a non‑zero length buffer fails */
        assert(src.len > 0);
        assert(dest.buffer == NULL);
        assert(dest.len == src.len);
        assert(dest.capacity == src.len);
        assert(dest.allocator == allocator);
        /* No validity guarantee is required on the error path */
    }
}
