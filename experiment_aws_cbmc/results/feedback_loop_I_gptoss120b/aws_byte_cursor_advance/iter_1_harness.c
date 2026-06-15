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
    /* 1. Declare and bound data structures */
    struct aws_byte_buf src;
    struct aws_byte_buf dest;
    struct aws_allocator *alloc = aws_default_allocator();

    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* 2. Save old state of src (dest is output, no need to save) */
    struct aws_byte_buf old_src = src;

    /* 3. Call function under test */
    int result = aws_byte_buf_init_copy(&dest, alloc, &src);

    /* 4. Assert postconditions for both success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        if (src.buffer == NULL) {
            /* src is empty, dest should be zeroed except allocator */
            assert(dest.buffer == NULL);
            assert(dest.len == 0);
            assert(dest.capacity == 0);
            assert(dest.allocator == alloc);
        } else {
            /* dest must contain a copy of src */
            assert(dest.buffer != NULL);
            assert(dest.len == src.len);
            assert(dest.capacity == src.capacity);
            assert(dest.allocator == alloc);
            /* the copied bytes must be identical */
            assert_bytes_match(dest.buffer, src.buffer, src.len);
        }
    } else {
        /* Allocation failure – dest must be zeroed */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
    }

    /* 5. Assert fields of src that must NOT change */
    assert(src.buffer == old_src.buffer);
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.allocator == old_src.allocator);

    /* 6. Validity invariants always hold */
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_buf_is_valid(&src));
}
