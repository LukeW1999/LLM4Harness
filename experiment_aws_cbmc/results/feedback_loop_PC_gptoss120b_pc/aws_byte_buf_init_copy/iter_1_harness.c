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
    /* 1. Declare and bound input structures */
    struct aws_byte_buf src;
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    struct aws_byte_buf dest; /* output buffer, uninitialized */

    struct aws_allocator *allocator = aws_default_allocator();

    /* 2. Save old state of src (inputs must stay unchanged) */
    struct aws_byte_buf old_src = src;
    struct store_byte_from_buffer src_storage;
    save_byte_from_array(src.buffer, src.capacity, &src_storage);

    /* 3. Call function under test */
    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    /* 4. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* dest must reflect a copy of src */
        assert(dest.allocator == allocator);
        assert(dest.len == src.len);
        assert(dest.capacity == src.capacity);

        if (src.buffer != NULL) {
            /* allocation succeeded, buffer must be non‑NULL */
            assert(dest.buffer != NULL);
            /* contents must be identical for the length of src */
            assert_bytes_match(dest.buffer, src.buffer, src.len);
        } else {
            /* when src has no buffer, dest must also have no buffer */
            assert(dest.buffer == NULL);
        }
    } else {
        /* on failure the destination is zeroed */
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.buffer == NULL);
        assert(dest.allocator == NULL);
    }

    /* 5. src must be unchanged regardless of result */
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.buffer == old_src.buffer);
    assert(src.allocator == old_src.allocator);
    assert_byte_from_buffer_matches(src.buffer, &src_storage);

    /* 6. Validity invariants must hold after the call */
    assert(aws_byte_buf_is_valid(&src));
    assert(aws_byte_buf_is_valid(&dest));
}
