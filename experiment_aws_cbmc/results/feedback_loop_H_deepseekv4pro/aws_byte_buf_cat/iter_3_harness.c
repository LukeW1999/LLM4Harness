#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>

void aws_byte_buf_cat_harness() {
    size_t number_of_args;
    __CPROVER_assume(number_of_args >= 2 && number_of_args <= 4);

    /* destination buffer */
    struct aws_byte_buf dest;
    dest.allocator = aws_default_allocator();
    dest.capacity = nondet_size_t();
    __CPROVER_assume(dest.capacity <= MAX_BUFFER_SIZE);
    dest.len = nondet_size_t();
    __CPROVER_assume(dest.len <= dest.capacity);
    dest.buffer = (uint8_t *)malloc(sizeof(uint8_t) * dest.capacity);
    __CPROVER_assume(dest.buffer != NULL || dest.capacity == 0);
    __CPROVER_assume(aws_byte_buf_is_valid(&dest));

    /* save old destination state */
    size_t old_len = dest.len;
    size_t old_capacity = dest.capacity;
    uint8_t *old_buffer = dest.buffer;
    struct aws_allocator *old_allocator = dest.allocator;

    /* source buffers */
    struct aws_byte_buf src[4];
    size_t total_src_len = 0;
    for (size_t i = 0; i < number_of_args; ++i) {
        src[i].allocator = aws_default_allocator();
        src[i].capacity = nondet_size_t();
        __CPROVER_assume(src[i].capacity <= MAX_BUFFER_SIZE);
        src[i].len = nondet_size_t();
        __CPROVER_assume(src[i].len <= src[i].capacity);
        src[i].buffer = (uint8_t *)malloc(sizeof(uint8_t) * src[i].capacity);
        __CPROVER_assume(src[i].buffer != NULL || src[i].capacity == 0);
        __CPROVER_assume(aws_byte_buf_is_valid(&src[i]));
        total_src_len += src[i].len;
    }

    /* save old source states */
    struct aws_byte_buf old_src[4];
    for (size_t i = 0; i < number_of_args; ++i) {
        old_src[i] = src[i];
    }
