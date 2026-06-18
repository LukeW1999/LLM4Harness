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

void aws_byte_buf_cat_harness(void) {
    /* 1. Destination buffer */
    struct aws_byte_buf dest;
    __CPROVER_assume(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&dest);
    __CPROVER_assume(aws_byte_buf_is_valid(&dest));

    /* Save old state of destination */
    struct aws_byte_buf old_dest = dest;

    /* 2. Number of source buffers (bounded) */
    size_t number_of_args = nondet_size_t();
    __CPROVER_assume(number_of_args <= 2); /* keep the harness small */

    /* 3. Source buffers */
    struct aws_byte_buf src1, src2;
    struct aws_byte_buf *src_ptrs[2] = {NULL, NULL};

    if (number_of_args >= 1) {
        __CPROVER_assume(aws_byte_buf_is_bounded(&src1, MAX_BUFFER_SIZE));
        ensure_byte_buf_has_allocated_buffer_member(&src1);
        __CPROVER_assume(aws_byte_buf_is_valid(&src1));
        src_ptrs[0] = &src1;
    }
    if (number_of_args == 2) {
        __CPROVER_assume(aws_byte_buf_is_bounded(&src2, MAX_BUFFER_SIZE));
        ensure_byte_buf_has_allocated_buffer_member(&src2);
        __CPROVER_assume(aws_byte_buf_is_valid(&src2));
        src_ptrs[1] = &src2;
    }

    /* Save old state of each source buffer */
    struct aws_byte_buf old_src1 = src1;
    struct aws_byte_buf old_src2 = src2;

    /* 4. Compute total length of all source buffers (used for post‑condition) */
    size_t total_len = 0;
    if (number_of_args >= 1) {
        total_len += src1.len;
    }
    if (number_of_args == 2) {
        total_len += src2.len;
    }

    /* 5. Call function under test */
    int result;
    if (number_of_args == 0) {
        result = aws_byte_buf_cat(&dest, 0);
    } else if (number_of_args == 1) {
        result = aws_byte_buf_cat(&dest, 1, src_ptrs[0]);
    } else {
        result = aws_byte_buf_cat(&dest, 2, src_ptrs[0], src_ptrs[1]);
    }

    /* 6. Post‑conditions */

    /* Unchanged fields of destination (always) */
    assert(dest.buffer == old_dest.buffer);
    assert(dest.capacity == old_dest.capacity);
    assert(dest.allocator == old_dest.allocator);

    if (result == AWS_OP_SUCCESS) {
        /* On success the length must increase by the total length of all sources */
        assert(dest.len == old_dest.len + total_len);
    } else {
        /* On failure the destination length must remain unchanged */
        assert(dest.len == old_dest.len);
    }

    /* Source buffers must remain unchanged */
    if (number_of_args >= 1) {
        assert(src1.buffer == old_src1.buffer);
        assert(src1.capacity == old_src1.capacity);
        assert(src1.allocator == old_src1.allocator);
        assert(src1.len == old_src1.len);
    }
    if (number_of_args == 2) {
        assert(src2.buffer == old_src2.buffer);
        assert(src2.capacity == old_src2.capacity);
        assert(src2.allocator == old_src2.allocator);
        assert(src2.len == old_src2.len);
    }

    /* 7. Validity invariants */
    assert(aws_byte_buf_is_valid(&dest));
    if (number_of_args >= 1) {
        assert(aws_byte_buf_is_valid(&src1));
    }
    if (number_of_args == 2) {
        assert(aws_byte_buf_is_valid(&src2));
    }
}
