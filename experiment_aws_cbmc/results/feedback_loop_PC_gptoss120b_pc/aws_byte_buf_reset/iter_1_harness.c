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

    struct aws_byte_buf dest; /* output, will be initialized by the function */

    struct aws_allocator *alloc = aws_default_allocator();

    /* 2. Save old state of src (immutable input) */
    struct aws_byte_buf old_src = src;
    struct store_byte_from_buffer src_byte_store;
    if (src.buffer != NULL && src.len > 0) {
        save_byte_from_array(src.buffer, src.len, &src_byte_store);
    }

    /* 3. Call function under test */
    int result = aws_byte_buf_init_copy(&dest, alloc, &src);

    /* 4. Postconditions for both success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* dest must reflect a successful copy */
        assert(dest.allocator == alloc);
        assert(dest.capacity == src.capacity);
        assert(dest.len == src.len);
        if (src.capacity > 0) {
            assert(dest.buffer != NULL);
        } else {
            assert(dest.buffer == NULL);
        }
        if (src.len > 0) {
            assert_bytes_match(dest.buffer, src.buffer, src.len);
        }

        /* src must be unchanged */
        assert(src.buffer == old_src.buffer);
        assert(src.len == old_src.len);
        assert(src.capacity == old_src.capacity);
        assert(src.allocator == old_src.allocator);
        if (src.buffer != NULL && src.len > 0) {
            assert_byte_from_buffer_matches(src.buffer, &src_byte_store);
        }

        /* validity invariants */
        assert(aws_byte_buf_is_valid(&dest));
        assert(aws_byte_buf_is_valid(&src));
    } else {
        /* on failure dest must be zeroed */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);

        /* src must be unchanged */
        assert(src.buffer == old_src.buffer);
        assert(src.len == old_src.len);
        assert(src.capacity == old_src.capacity);
        assert(src.allocator == old_src.allocator);
        if (src.buffer != NULL && src.len > 0) {
            assert_byte_from_buffer_matches(src.buffer, &src_byte_store);
        }

        /* validity invariants */
        assert(aws_byte_buf_is_valid(&dest));
        assert(aws_byte_buf_is_valid(&src));
    }
}
