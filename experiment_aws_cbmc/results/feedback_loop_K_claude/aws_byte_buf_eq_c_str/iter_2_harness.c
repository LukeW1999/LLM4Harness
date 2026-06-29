#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#ifndef MAX_BUF_SIZE
#define MAX_BUF_SIZE 8
#endif

void aws_byte_buf_eq_c_str_harness(void) {
    struct aws_byte_buf buf;

    size_t len;
    size_t capacity;
    __CPROVER_assume(len <= MAX_BUF_SIZE);
    __CPROVER_assume(capacity >= len);
    __CPROVER_assume(capacity <= MAX_BUF_SIZE);

    buf.len = len;
    buf.capacity = capacity;
    buf.allocator = aws_default_allocator();

    if (capacity > 0) {
        buf.buffer = malloc(capacity);
        __CPROVER_assume(buf.buffer != NULL);
    } else {
        buf.buffer = NULL;
    }

    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    size_t c_str_len;
    __CPROVER_assume(c_str_len <= MAX_BUF_SIZE);

    char *c_str = malloc(c_str_len + 1);
    __CPROVER_assume(c_str != NULL);

    /* Ensure c_str has no embedded nulls so strlen(c_str) == c_str_len */
    for (size_t i = 0; i < c_str_len; i++) {
        __CPROVER_assume(c_str[i] != '\0');
    }
    c_str[c_str_len] = '\0';

    size_t old_len = buf.len;
    size_t old_capacity = buf.capacity;
    uint8_t *old_buffer = buf.buffer;

    bool result = aws_byte_buf_eq_c_str(&buf, c_str);

    assert(buf.len == old_len);
    assert(buf.capacity == old_capacity);
    assert(buf.buffer == old_buffer);

    assert(result == true || result == false);

    if (buf.len != c_str_len) {
        assert(!result);
    }

    if (result) {
        assert(buf.len == c_str_len);
        for (size_t i = 0; i < buf.len; i++) {
            assert(buf.buffer[i] == (uint8_t)c_str[i]);
        }
    }
}
