#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include <aws/common/common.h>
#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 256

void aws_string_eq_byte_buf_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_string *str;
    if (nondet_bool()) {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);
        uint8_t storage[sizeof(struct aws_string) + MAX_BUFFER_SIZE];
        str = (struct aws_string *)storage;
        str->allocator = allocator;
        str->len = len;
        /* bytes are left nondet */
    } else {
        str = NULL;
    }

    struct aws_byte_buf buf;
    AWS_ZERO_STRUCT(buf);
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    buf.allocator = allocator;
    buf.len = nondet_size_t();
    __CPROVER_assume(buf.len <= buf.capacity);

    struct aws_string *old_str_ptr = str;
    struct aws_allocator *old_str_allocator = NULL;
    size_t old_str_len = 0;
    if (str) {
        old_str_allocator = str->allocator;
        old_str_len = str->len;
    }

    struct aws_byte_buf old_buf = buf;

    bool result = aws_string_eq_byte_buf(str, &buf);

    if (str == NULL) {
        assert(result == false);
    } else {
        bool expected = (str->len == buf.len) &&
                        (memcmp(str->bytes, buf.buffer, str->len) == 0);
        assert(result == expected);
    }

    if (str) {
        assert(str->allocator == old_str_allocator);
        assert(str->len == old_str_len);
    } else {
        assert(str == old_str_ptr);
    }

    assert(buf.allocator == old_buf.allocator);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.buffer == old_buf.buffer);
    assert(buf.len == old_buf.len);

    if (str) {
        assert(aws_string_is_valid(str));
    }
    assert(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
}
