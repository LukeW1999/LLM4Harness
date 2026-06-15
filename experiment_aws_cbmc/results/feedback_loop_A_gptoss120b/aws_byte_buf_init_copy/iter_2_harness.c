#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include "aws/common/byte_buf.h"
#include "aws/common/allocator.h"
#include "proof_helpers/make_common_data_structures.h"

void aws_byte_buf_init_copy_harness(void) {
    /* 1. Declare data structures */
    struct aws_byte_buf src;
    struct aws_byte_buf dest;
    struct aws_allocator *allocator;

    /* 2. Nondeterministically set source buffer */
    src.buffer = nondet_bool() ? NULL : malloc(MAX_BUFFER_SIZE);
    src.capacity = nondet_uint() % (MAX_BUFFER_SIZE + 1);
    src.len = src.buffer == NULL ? 0 : nondet_uint() % (src.capacity + 1);
    src.allocator = NULL; /* allocator of src is irrelevant for copy */

    /* 3. Assume src is bounded and valid */
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* 4. Nondeterministically choose allocator (NULL or a fresh struct) */
    allocator = nondet_bool() ? NULL : malloc(sizeof(struct aws_allocator));

    /* 5. Save old state of src for later comparison */
    struct aws_byte_buf old_src = src;

    /* 6. Call function under test */
    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    /* 7. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        if (src.buffer == NULL) {
            /* Source empty → dest should be an empty buffer */
            assert(dest.len == 0);
            assert(dest.capacity == 0);
            assert(dest.buffer == NULL);
            assert(dest.allocator == allocator);
        } else {
            /* Normal copy */
            assert(dest.len == src.len);
            assert(dest.capacity == src.capacity);
            assert(dest.buffer != NULL);
            assert(dest.buffer != src.buffer);
            assert_bytes_match(dest.buffer, src.buffer, src.len);
            assert(dest.allocator == allocator);
        }
    } else {
        /* Allocation failure – dest must be zeroed */
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.buffer == NULL);
        assert(dest.allocator == NULL);
    }

    /* 8. src must remain unchanged */
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.buffer == old_src.buffer);
    assert(src.allocator == old_src.allocator);

    /* 9. Invariants */
    assert(aws_byte_buf_is_valid(&src));
    assert(aws_byte_buf_is_valid(&dest));
}
