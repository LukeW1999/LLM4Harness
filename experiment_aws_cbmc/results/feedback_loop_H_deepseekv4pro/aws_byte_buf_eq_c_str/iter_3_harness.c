#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 256

void aws_byte_buf_eq_c_str_harness() {
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    size_t c_str_len;
    bool is_null;
    __CPROVER_assume(is_null == 0 || is_null == 1);
    const char *c_str;
    if (is_null) {
        c_str = NULL;
    } else {
        __CPROVER_assume(c_str_len <= MAX_BUFFER_SIZE);
        c_str = (const char *)malloc(c_str_len + 1);
        if (c_str != NULL) {
            c_str[c_str_len] = '\0';
        }
    }

    struct aws_byte_buf old = buf;

    bool result = aws_byte_buf_eq_c_str(&buf, c_str);

    assert(buf.len == old.len);
    assert(buf.capacity == old.capacity);
    assert(buf.buffer == old.buffer);
    assert(buf.allocator == old.allocator);
    assert(aws_byte_buf_is_valid(&buf));

    if (!is_null && c_str != NULL) {
        free((void *)c_str);
    }
}
