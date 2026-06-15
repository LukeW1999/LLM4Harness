#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <aws/common/assert.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "proof_helpers/make_common_data_structures.h"

#define MAX_ARGS 3

void aws_byte_buf_cat_harness(void) {
    /* allocator */
    struct aws_allocator *alloc = aws_default_allocator();

    /* Destination buffer */
    struct aws_byte_buf dest;
    size_t dest_capacity = nondet_size_t();
    __CPROVER_assume(dest_capacity <= 1024);               /* bound for CBMC */
    __CPROVER_assume(aws_byte_buf_init(&dest, alloc, dest_capacity) == AWS_OP_SUCCESS);
    dest.len = nondet_size_t();
    __CPROVER_assume(dest.len <= dest.capacity);

    /* Save a copy of destination state */
    struct aws_byte_buf dest_before = dest;
    uint8_t *dest_buf_copy = NULL;
    if (dest.buffer != NULL && dest.capacity > 0) {
        dest_buf_copy = malloc(dest.capacity);
        __CPROVER_assume(dest_buf_copy != NULL);
        memcpy(dest_buf_copy, dest.buffer, dest.capacity);
    }

    /* Source buffers */
    struct aws_byte_buf *src_buf[MAX_ARGS];
    for (size_t i = 0; i < MAX_ARGS; ++i) {
        src_buf[i] = malloc(sizeof(struct aws_byte_buf));
        __CPROVER_assume(src_buf[i] != NULL);
        size_t cap = nondet_size_t();
        __CPROVER_assume(cap <= 1024);
        __CPROVER_assume(aws_byte_buf_init(src_buf[i], alloc, cap) == AWS_OP_SUCCESS);
        src_buf[i]->len = nondet_size_t();
        __CPROVER_assume(src_buf[i]->len <= src_buf[i]->capacity);
    }

    /* Save copies of source states */
    struct aws_byte_buf src_before[MAX_ARGS];
    uint8_t *src_buf_copy[MAX_ARGS];
    for (size_t i = 0; i < MAX_ARGS; ++i) {
        src_before[i] = *src_buf[i];
        src_buf_copy[i] = NULL;
        if (src_buf[i]->buffer != NULL && src_buf[i]->capacity > 0) {
            src_buf_copy[i] = malloc(src_buf[i]->capacity);
            __CPROVER_assume(src_buf_copy[i] != NULL);
            memcpy(src_buf_copy[i], src_buf[i]->buffer, src_buf[i]->capacity);
        }
    }

    /* Number of arguments */
    size_t number_of_args = nondet_size_t();
    __CPROVER_assume(number_of_args >= 1 && number_of_args <= MAX_ARGS);

    /* Call the function with the appropriate var‑args */
    int ret;
    switch (number_of_args) {
        case 1:
            ret = aws_byte_buf_cat(&dest, number_of_args,
                                   src_buf[0]);
            break;
        case 2:
            ret = aws_byte_buf_cat(&dest, number_of_args,
                                   src_buf[0], src_buf[1]);
            break;
        case 3:
            ret = aws_byte_buf_cat(&dest, number_of_args,
                                   src_buf[0], src_buf[1], src_buf[2]);
            break;
        default:
            /* unreachable because of the assume above */
            ret = AWS_OP_ERR;
            break;
    }

    /* Postcondition: return value */
    assert(ret == AWS_OP_SUCCESS || ret == AWS_OP_ERR);

    /* Postcondition: dest validity */
    assert(aws_byte_buf_is_valid(&dest));

    /* Compute total source length */
    size_t total_src_len = 0;
    for (size_t i = 0; i < number_of_args; ++i) {
        total_src_len += src_buf[i]->len;
    }

    /* Length invariants */
    assert(dest.len <= dest_before.len + total_src_len);
    assert(dest.len <= dest.capacity);
    if (ret == AWS_OP_SUCCESS) {
        assert(dest.len == dest_before.len + total_src_len);
        assert(total_src_len <= dest.capacity);
    }

    /* Destination buffer pointer unchanged */
    assert(dest.buffer == dest_before.buffer);

    /* Source buffers unchanged */
    for (size_t i = 0; i < number_of_args; ++i) {
        assert(src_buf[i]->len == src_before[i].len);
        assert(src_buf[i]->capacity == src_before[i].capacity);
        if (src_buf[i]->buffer != NULL && src_before[i].capacity > 0) {
            assert(memcmp(src_buf[i]->buffer, src_buf_copy[i],
                          src_before[i].capacity) == 0);
        }
    }

    /* Clean up */
    if (dest_buf_copy) free(dest_buf_copy);
    for (size_t i = 0; i < MAX_ARGS; ++i) {
        if (src_buf_copy[i]) free(src_buf_copy[i]);
        if (src_buf[i]) {
            aws_byte_buf_clean_up(src_buf[i]);
            free(src_buf[i]);
        }
    }
    aws_byte_buf_clean_up(&dest);

    return 0;
}
