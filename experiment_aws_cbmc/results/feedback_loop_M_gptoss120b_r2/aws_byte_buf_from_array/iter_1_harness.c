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

    struct aws_byte_buf dest; /* output, no pre‑condition required */

    struct aws_allocator *alloc = aws_default_allocator();

    /* 2. Save old state of src (src must remain unchanged) */
    struct aws_byte_buf old_src = src;
    struct store_byte_from_buffer old_src_bytes = {0};
    if (src.buffer != NULL && src.len > 0) {
        save_byte_from_array(src.buffer, src.len, &old_src_bytes);
    }

    /* 3. Call function under test */
    int result = aws_byte_buf_init_copy(&dest, alloc, &src);

    /* 4. Post‑condition: dest must always be a valid byte buffer */
    assert(aws_byte_buf_is_valid(&dest));

    /* 5. src must be unchanged regardless of result */
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.buffer == old_src.buffer);
    assert(src.allocator == old_src.allocator);
    assert(aws_byte_buf_is_valid(&src));

    if (result == AWS_OP_SUCCESS) {
        /* Success path */
        if (old_src.buffer == NULL) {
            /* src had no buffer – dest should be zeroed but with allocator set */
            assert(dest.len == 0);
            assert(dest.capacity == 0);
            assert(dest.buffer == NULL);
            assert(dest.allocator == alloc);
        } else {
            /* src had a buffer – dest should contain a copy */
            assert(dest.len == old_src.len);
            assert(dest.capacity == old_src.capacity);
            assert(dest.buffer != NULL);
            assert(dest.allocator == alloc);
            /* Verify that the copied bytes match the original */
            if (old_src.len > 0) {
                assert_bytes_match(dest.buffer, old_src.buffer, old_src.len);
            }
        }
    } else {
        /* Failure path – allocation failed, dest must be zeroed */
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.buffer == NULL);
        assert(dest.allocator == NULL);
    }
}
