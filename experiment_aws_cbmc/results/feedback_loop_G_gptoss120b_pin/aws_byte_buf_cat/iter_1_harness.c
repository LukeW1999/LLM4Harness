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

#define MAX_ARGS 5

void aws_byte_buf_cat_harness(void) {
    /* 1. Declare and bound dest buffer */
    struct aws_byte_buf dest;
    __CPROVER_assume(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&dest);
    __CPROVER_assume(aws_byte_buf_is_valid(&dest));

    /* 2. Declare and bound source buffers */
    struct aws_byte_buf src[MAX_ARGS];
    for (size_t i = 0; i < MAX_ARGS; ++i) {
        __CPROVER_assume(aws_byte_buf_is_bounded(&src[i], MAX_BUFFER_SIZE));
        ensure_byte_buf_has_allocated_buffer_member(&src[i]);
        __CPROVER_assume(aws_byte_buf_is_valid(&src[i]));
    }

    /* 3. Non‑deterministic number of arguments (bounded) */
    size_t number_of_args = nondet_size_t();
    __CPROVER_assume(number_of_args <= MAX_ARGS);

    /* 4. Save old state */
    struct aws_byte_buf old_dest = dest;
    struct aws_byte_buf old_src[MAX_ARGS];
    for (size_t i = 0; i < MAX_ARGS; ++i) {
        old_src[i] = src[i];
    }

    /* 5. Call function under test with appropriate var‑args */
    int result;
    if (number_of_args == 0) {
        result = aws_byte_buf_cat(&dest, 0);
    } else if (number_of_args == 1) {
        result = aws_byte_buf_cat(&dest, 1, &src[0]);
    } else if (number_of_args == 2) {
        result = aws_byte_buf_cat(&dest, 2, &src[0], &src[1]);
    } else if (number_of_args == 3) {
        result = aws_byte_buf_cat(&dest, 3, &src[0], &src[1], &src[2]);
    } else if (number_of_args == 4) {
        result = aws_byte_buf_cat(&dest, 4, &src[0], &src[1], &src[2], &src[3]);
    } else { /* number_of_args == 5 */
        result = aws_byte_buf_cat(&dest, 5, &src[0], &src[1], &src[2], &src[3], &src[4]);
    }

    /* 6. Compute total length of all source buffers */
    size_t total_src_len = 0;
    for (size_t i = 0; i < number_of_args; ++i) {
        total_src_len += src[i].len;
    }

    /* 7. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* dest.len increased by sum of source lengths */
        assert(dest.len == old_dest.len + total_src_len);
        /* dest.buffer pointer unchanged (no reallocation in plain append) */
        assert(dest.buffer == old_dest.buffer);
        /* dest.capacity unchanged */
        assert(dest.capacity == old_dest.capacity);
        /* dest.allocator unchanged */
        assert(dest.allocator == old_dest.allocator);
    } else {
        /* on failure dest must be unchanged */
        assert(dest.len == old_dest.len);
        assert(dest.buffer == old_dest.buffer);
        assert(dest.capacity == old_dest.capacity);
        assert(dest.allocator == old_dest.allocator);
    }

    /* 8. Source buffers must remain unchanged regardless of result */
    for (size_t i = 0; i < MAX_ARGS; ++i) {
        assert(src[i].len == old_src[i].len);
        assert(src[i].buffer == old_src[i].buffer);
        assert(src[i].capacity == old_src[i].capacity);
        assert(src[i].allocator == old_src[i].allocator);
    }

    /* 9. Validity invariants must hold after the call */
    assert(aws_byte_buf_is_valid(&dest));
    for (size_t i = 0; i < MAX_ARGS; ++i) {
        assert(aws_byte_buf_is_valid(&src[i]));
    }

    return 0;
}
