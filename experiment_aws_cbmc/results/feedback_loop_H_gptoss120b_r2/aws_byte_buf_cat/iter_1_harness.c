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
    /* destination buffer */
    struct aws_byte_buf dest;
    __CPROVER_assume(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&dest);
    __CPROVER_assume(aws_byte_buf_is_valid(&dest));

    /* source buffers */
    struct aws_byte_buf src[MAX_ARGS];
    for (size_t i = 0; i < MAX_ARGS; ++i) {
        __CPROVER_assume(aws_byte_buf_is_bounded(&src[i], MAX_BUFFER_SIZE));
        ensure_byte_buf_has_allocated_buffer_member(&src[i]);
        __CPROVER_assume(aws_byte_buf_is_valid(&src[i]));
    }

    /* number of arguments (must be > 1 per Doxygen) */
    size_t number_of_args = nondet_size_t();
    __CPROVER_assume(number_of_args >= 2);
    __CPROVER_assume(number_of_args <= MAX_ARGS);

    /* save old state */
    struct aws_byte_buf old_dest = dest;
    struct aws_byte_buf old_src[MAX_ARGS];
    for (size_t i = 0; i < MAX_ARGS; ++i) {
        old_src[i] = src[i];
    }

    /* call function under test */
    int result = aws_byte_buf_cat(&dest,
                                  number_of_args,
                                  &src[0],
                                  &src[1],
                                  &src[2],
                                  &src[3],
                                  &src[4]);

    /* post‑conditions */
    if (result == AWS_OP_SUCCESS) {
        /* compute expected length */
        size_t expected_len = old_dest.len;
        for (size_t i = 0; i < number_of_args; ++i) {
            expected_len += src[i].len;
        }
        assert(dest.len == expected_len);
        /* capacity may have grown, but must be at least len */
        assert(dest.capacity >= dest.len);
        /* allocator must stay the same */
        assert(dest.allocator == old_dest.allocator);
    } else {
        /* on error, dest may have been partially updated */
        /* len cannot exceed what would be written on full success */
        size_t max_possible_len = old_dest.len;
        for (size_t i = 0; i < number_of_args; ++i) {
            max_possible_len += src[i].len;
        }
        assert(dest.len <= max_possible_len);
        assert(dest.len >= old_dest.len);
        /* capacity and buffer pointer must not change on failure */
        assert(dest.capacity == old_dest.capacity);
        assert(dest.buffer == old_dest.buffer);
        /* allocator unchanged */
        assert(dest.allocator == old_dest.allocator);
    }

    /* source buffers must be unchanged */
    for (size_t i = 0; i < MAX_ARGS; ++i) {
        assert(src[i].len == old_src[i].len);
        assert(src[i].capacity == old_src[i].capacity);
        assert(src[i].buffer == old_src[i].buffer);
        assert(src[i].allocator == old_src[i].allocator);
    }

    /* validity invariants */
    assert(aws_byte_buf_is_valid(&dest));
    for (size_t i = 0; i < MAX_ARGS; ++i) {
        assert(aws_byte_buf_is_valid(&src[i]));
    }
}
