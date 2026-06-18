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
    /* allocator (default, non‑null) */
    struct aws_allocator *alloc = aws_default_allocator();

    /* source buffer */
    struct aws_byte_buf src;
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    /* nondeterministically decide whether the source has a NULL buffer */
    if (nondet_bool()) {
        src.buffer = NULL;
        src.len = 0;
        src.capacity = 0;
    } else {
        __CPROVER_assume(src.len <= src.capacity);
    }
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* keep a copy of src for immutability checks (structure only) */
    struct aws_byte_buf old_src = src;

    /* destination buffer (uninitialized) */
    struct aws_byte_buf dest;

    /* call the function under verification */
    int result = aws_byte_buf_init_copy(&dest, alloc, &src);

    /* post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        if (src.buffer == NULL) {
            /* when source buffer is NULL the destination must be a zeroed buf */
            assert(dest.buffer == NULL);
            assert(dest.len == 0);
            assert(dest.capacity == 0);
        } else {
            /* destination must have a newly allocated, non‑NULL buffer */
            assert(dest.buffer != NULL);
            assert(dest.buffer != src.buffer);
            assert(dest.len == src.len);
            assert(dest.capacity == src.capacity);
            assert(dest.allocator == alloc);
            assert_bytes_match(dest.buffer, src.buffer, src.len);
        }
    } else {
        /* failure path – allocation failed, destination is zeroed */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
    }

    /* source buffer must remain unchanged (structure fields) */
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.buffer == old_src.buffer);
    assert(src.allocator == old_src.allocator);

    /* validity invariants */
    assert(aws_byte_buf_is_valid(&src));
    assert(aws_byte_buf_is_valid(&dest));
}
