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
    /* 1. Declare and bound destination buffer */
    struct aws_byte_buf dest;
    __CPROVER_assume(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&dest);
    __CPROVER_assume(aws_byte_buf_is_valid(&dest));

    /* 2. Declare and bound source buffers (maximum 3 arguments) */
    const size_t MAX_ARGS = 3;
    struct aws_byte_buf src[MAX_ARGS];
    for (size_t i = 0; i < MAX_ARGS; ++i) {
        __CPROVER_assume(aws_byte_buf_is_bounded(&src[i], MAX_BUFFER_SIZE));
        ensure_byte_buf_has_allocated_buffer_member(&src[i]);
        __CPROVER_assume(aws_byte_buf_is_valid(&src[i]));
    }

    /* 3. Save old state */
    struct aws_byte_buf old_dest = dest;
    struct aws_byte_buf old_src[MAX_ARGS];
    for (size_t i = 0; i < MAX_ARGS; ++i) {
        old_src[i] = src[i];
    }

    /* 4. Nondeterministic number of arguments (bounded) */
    size_t number_of_args = nondet_size_t();
    __CPROVER_assume(number_of_args <= MAX_ARGS);

    /* 5. Call function under test */
    int result = aws_byte_buf_cat(&dest,
                                 number_of_args,
                                 &src[0],
                                 &src[1],
                                 &src[2]);

    /* 6. Compute total length of processed source buffers */
    size_t total_src_len = 0;
    for (size_t i = 0; i < number_of_args; ++i) {
        total_src_len += src[i].len;
    }

    /* 7. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* All source data must have been appended */
        assert(dest.len == old_dest.len + total_src_len);
        /* No reallocation performed by plain append */
        assert(dest.buffer == old_dest.buffer);
        assert(dest.capacity == old_dest.capacity);
        assert(dest.allocator == old_dest.allocator);
    } else {
        /* On failure, dest must remain within its original capacity */
        assert(dest.capacity == old_dest.capacity);
        assert(dest.buffer == old_dest.buffer);
        assert(dest.allocator == old_dest.allocator);
        /* Length cannot exceed original length plus total source length and cannot exceed capacity */
        assert(dest.len <= old_dest.len + total_src_len);
        assert(dest.len <= dest.capacity);
    }

    /* 8. Source buffers must be unchanged */
    for (size_t i = 0; i < MAX_ARGS; ++i) {
        assert(src[i].len == old_src[i].len);
        assert(src[i].capacity == old_src[i].capacity);
        assert(src[i].buffer == old_src[i].buffer);
        assert(src[i].allocator == old_src[i].allocator);
    }

    /* 9. Validity invariants always hold */
    assert(aws_byte_buf_is_valid(&dest));
    for (size_t i = 0; i < MAX_ARGS; ++i) {
        assert(aws_byte_buf_is_valid(&src[i]));
    }
}
