#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

void aws_byte_buf_eq_c_str_harness(void) {
    /* allocator */
    struct aws_allocator *allocator = aws_default_allocator();

    /* aws_byte_buf input */
    struct aws_byte_buf buf;
    buf.len = nondet_uint();
    buf.capacity = nondet_uint();
    __CPROVER_assume(buf.capacity >= buf.len);

    if (buf.capacity > 0) {
        buf.buffer = malloc(buf.capacity);
        __CPROVER_assume(buf.buffer != NULL);
    } else {
        buf.buffer = NULL;
    }
    buf.allocator = allocator;

    /* c_str input */
    size_t c_str_len = nondet_uint();
    char *c_str = malloc(c_str_len + 1);
    __CPROVER_assume(c_str != NULL);
    c_str[c_str_len] = '\0';

    /* preconditions */
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* snapshot of inputs */
    size_t old_len = buf.len;
    size_t old_capacity = buf.capacity;
    uint8_t *old_buffer = NULL;
    if (buf.buffer != NULL && old_len > 0) {
        old_buffer = malloc(old_len);
        __CPROVER_assume(old_buffer != NULL);
        memcpy(old_buffer, buf.buffer, old_len);
    }
    size_t old_c_str_len = c_str_len;
    char *old_c_str = malloc(old_c_str_len + 1);
    __CPROVER_assume(old_c_str != NULL);
    memcpy(old_c_str, c_str, old_c_str_len + 1);

    /* call under test */
    bool result = aws_byte_buf_eq_c_str(&buf, c_str);

    /* ASSERT_POSTCONDITIONS_HERE */
}
