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

    /* 3. Create and initialize source buffers */
    struct aws_byte_buf *src_bufs[4];
    for (size_t i = 0; i < number_of_args; i++) {
        src_bufs[i] = malloc(sizeof(struct aws_byte_buf));
        __CPROVER_assume(src_bufs[i] != NULL);
        src_bufs[i]->allocator = aws_default_allocator();
        __CPROVER_assume(aws_byte_buf_is_bounded(src_bufs[i], MAX_BUFFER_SIZE));
        ensure_byte_buf_has_allocated_buffer_member(src_bufs[i]);
        __CPROVER_assume(aws_byte_buf_is_valid(src_bufs[i]));
    }

    /* 4. Save old states */
    struct aws_byte_buf old_dest = dest;
    struct store_byte_from_buffer old_dest_byte;
    if (dest.buffer != NULL && dest.capacity > 0) {
        save_byte_from_array(dest.buffer, dest.capacity, &old_dest_byte);
    }

    struct aws_byte_buf old_src_bufs[4];
    struct store_byte_from_buffer old_src_bytes[4];
    for (size_t i = 0; i < number_of_args; i++) {
        old_src_bufs[i] = *src_bufs[i];
        if (src_bufs[i]->buffer != NULL && src_bufs[i]->capacity > 0) {
            save_byte_from_array(src_bufs[i]->buffer, src_bufs[i]->capacity, &old_src_bytes[i]);
        }
    }

    /* 5. Call the function under test */
    int result;
    switch (number_of_args) {
        case 1:
            result = aws_byte_buf_cat(&dest, 1, src_bufs[0]);
            break;
        case 2:
            result = aws_byte_buf_cat(&dest, 2, src_bufs[0], src_bufs[1]);
            break;
        case 3:
            result = aws_byte_buf_cat(&dest, 3, src_bufs[0], src_bufs[1], src_bufs[2]);
            break;
        case 4:
            result = aws_byte_buf_cat(&dest, 4, src_bufs[0], src_bufs[1], src_bufs[2], src_bufs[3]);
            break;
        default:
            /* unreachable because number_of_args is bounded 1..4 */
            result = aws_byte_buf_cat(&dest, number_of_args, src_bufs[0]);
            break;
    }
