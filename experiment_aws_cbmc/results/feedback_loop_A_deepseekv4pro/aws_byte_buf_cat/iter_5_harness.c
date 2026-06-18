#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 1000
#endif

void aws_byte_buf_cat_harness() {
    /* 1. Set up destination buffer */
    struct aws_byte_buf dest;
    dest.allocator = aws_default_allocator();
    __CPROVER_assume(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&dest);
    __CPROVER_assume(aws_byte_buf_is_valid(&dest));

    /* 2. Choose number of source buffers (at least 1, bounded) */
    size_t number_of_args;
    __CPROVER_assume(number_of_args >= 1);
    __CPROVER_assume(number_of_args <= 4);

    /* 3. Create and initialize source buffers (stack allocated) */
    struct aws_byte_buf src[4];
    for (size_t i = 0; i < number_of_args; i++) {
        src[i].allocator = aws_default_allocator();
        __CPROVER_assume(aws_byte_buf_is_bounded(&src[i], MAX_BUFFER_SIZE));
        ensure_byte_buf_has_allocated_buffer_member(&src[i]);
        __CPROVER_assume(aws_byte_buf_is_valid(&src[i]));
    }

    /* 4. Save old states */
    struct aws_byte_buf old_dest = dest;
    struct store_byte_from_buffer old_dest_byte;
    if (dest.buffer != NULL && dest.capacity > 0) {
        save_byte_from_array(dest.buffer, dest.capacity, &old_dest_byte);
    }

    struct aws_byte_buf old_src[4];
    struct store_byte_from_buffer old_src_byte[4];
    for (size_t i = 0; i < number_of_args; i++) {
        old_src[i] = src[i];
        if (src[i].buffer != NULL && src[i].capacity > 0) {
            save_byte_from_array(src[i].buffer, src[i].capacity, &old_src_byte[i]);
        }
    }

    /* 5. Sum of source lens */
    size_t sum_src_len = 0;
    for (size_t i = 0; i < number_of_args; i++) {
        sum_src_len += src[i].len;
    }

    /* 6. Call the function under test */
    int result;
    switch (number_of_args) {
        case 1:
            result = aws_byte_buf_cat(&dest, 1, &src[0]);
            break;
        case 2:
            result = aws_byte_buf_cat(&dest, 2, &src[0], &src[1]);
            break;
        case 3:
            result = aws_byte_buf_cat(&dest, 3, &src[0], &src[1], &src[2]);
            break;
        case 4:
            result = aws_byte_buf_cat(&dest, 4, &src[0], &src[1], &src[2], &src[3]);
            break;
        default:
            result = aws_byte_buf_cat(&dest, 1, &src[0]);
            break;
    }

    /* 7. Postconditions */
    if (result == AWS_OP_SUCCESS) {
        assert(dest.len == old_dest.len + sum_src_len);
        assert(dest.capacity == old_dest.capacity);
        /* check that dest buffer content is old_dest followed by src buffers */
        if (dest.buffer != NULL) {
            assert_bytes_match(old_dest.buffer, dest.buffer, old_dest.len);
            size_t offset = old_dest.len;
            for (size_t i = 0; i < number_of_args; i++) {
                if (src[i].buffer != NULL) {
                    assert_bytes_match(src[i].buffer, dest.buffer + offset, src[i].len);
                    offset += src[i].len;
                }
            }
        }
    } else {
        /* On error, dest should remain unchanged */
        assert(dest.len == old_dest.len);
        assert(dest.capacity == old_dest.capacity);
        if (dest.buffer != NULL) {
            assert_bytes_match(old_dest.buffer, dest.buffer, old_dest.len);
        }
    }

    /* Source buffers must not have been modified */
    for (size_t i = 0; i < number_of_args; i++) {
        assert(old_src[i].len == src[i].len);
        assert(old_src[i].capacity == src[i].capacity);
        if (src[i].buffer != NULL) {
            assert_bytes_match(old_src[i].buffer, src[i].buffer, src[i].len);
        }
    }

    /* Check that buffers were not overrun */
    if (dest.buffer != NULL) {
        check_bytes_not_written(&old_dest_byte, dest.buffer, dest.capacity);
    }
    for (size_t i = 0; i < number_of_args; i++) {
        if (src[i].buffer != NULL) {
            check_bytes_not_written(&old_src_byte[i], src[i].buffer, src[i].capacity);
        }
    }
}
