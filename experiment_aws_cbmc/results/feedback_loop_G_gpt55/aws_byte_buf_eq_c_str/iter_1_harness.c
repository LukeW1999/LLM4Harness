#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

static bool byte_buf_contents_equal_c_str(
    const struct aws_byte_buf *const buf,
    const char *const c_str,
    size_t c_str_len) {
    if (buf->len != c_str_len) {
        return false;
    }

    for (size_t i = 0; i < buf->len; ++i) {
        if (buf->buffer[i] != (uint8_t)c_str[i]) {
            return false;
        }
    }

    return true;
}

void aws_byte_buf_eq_c_str_harness() {
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    size_t c_str_len = nondet_size_t();
    __CPROVER_assume(c_str_len <= MAX_BUFFER_SIZE);
    __CPROVER_assume(c_str_len < SIZE_MAX);

    char *c_str = malloc(c_str_len + 1);
    __CPROVER_assume(c_str != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(c_str, c_str_len + 1));

    for (size_t i = 0; i < c_str_len; ++i) {
        __CPROVER_assume(c_str[i] != '\0');
    }
    c_str[c_str_len] = '\0';

    struct aws_byte_buf old = buf;

    struct store_byte_from_buffer old_buf_byte;
    save_byte_from_array(old.buffer, old.len, &old_buf_byte);

    struct store_byte_from_buffer old_c_str_byte;
    save_byte_from_array((const uint8_t *)c_str, c_str_len + 1, &old_c_str_byte);

    bool expected = byte_buf_contents_equal_c_str(&old, c_str, c_str_len);

    bool result = aws_byte_buf_eq_c_str(&buf, c_str);

    if (result) {
        assert(expected);
    } else {
        assert(!expected);
    }

    assert(buf.len == old.len);
    assert(buf.buffer == old.buffer);
    assert(buf.capacity == old.capacity);
    assert(buf.allocator == old.allocator);

    assert_byte_from_buffer_matches(buf.buffer, &old_buf_byte);
    assert_byte_from_buffer_matches((const uint8_t *)c_str, &old_c_str_byte);

    assert(aws_byte_buf_is_valid(&buf));
    assert(AWS_MEM_IS_READABLE(c_str, c_str_len + 1));
}
