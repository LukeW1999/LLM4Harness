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
    /* 1. Declare structures and allocator */
    struct aws_byte_buf dest;
    struct aws_byte_buf src;
    struct aws_allocator *alloc = aws_default_allocator();

    /* 2. Bound and allocate source buffer, then assume it is valid */
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* 3. Save old state of source (frame) */
    struct aws_byte_buf old_src = src;
    struct store_byte_from_buffer src_bytes;
    if (src.buffer != NULL && src.len > 0) {
        save_byte_from_array(src.buffer, src.len, &src_bytes);
    }

    /* 4. Call function under test */
    int result = aws_byte_buf_init_copy(&dest, alloc, &src);

    /* 5. Postconditions for success path */
    if (result == AWS_OP_SUCCESS) {
        /* dest must be a valid byte buffer */
        assert(aws_byte_buf_is_valid(&dest));

        /* allocator is set to the one passed in */
        assert(dest.allocator == alloc);

        /* capacity and length are copied from src */
        assert(dest.capacity == src.capacity);
        assert(dest.len == src.len);

        if (src.buffer == NULL) {
            /* when src has no buffer, dest.buffer must be NULL */
            assert(dest.buffer == NULL);
        } else {
            /* when src has a buffer, dest.buffer must be non‑NULL */
            assert(dest.buffer != NULL);
            /* the contents of the first src.len bytes must be identical */
            assert_bytes_match(src.buffer, dest.buffer, src.len);
        }
    } else {
        /* 6. Postconditions for failure path */
        /* on allocation failure the destination is zeroed */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
        /* zeroed destination is still a valid byte buffer */
        assert(aws_byte_buf_is_valid(&dest));
    }

    /* 7. Frame conditions: src must remain unchanged */
    assert(src.buffer == old_src.buffer);
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.allocator == old_src.allocator);
    assert(aws_byte_buf_is_valid(&src));
}
