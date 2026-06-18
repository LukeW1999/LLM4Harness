#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>

#ifndef MAX_BUFFER_SIZE
#    define MAX_BUFFER_SIZE 16
#endif

struct aws_string *make_arbitrary_aws_string_nondet_len_with_max(size_t max_len) {
    size_t len;
    __CPROVER_assume(len <= max_len);
    struct aws_string *str = malloc(sizeof(struct aws_string) + len + 1);
    __CPROVER_assume(str != NULL);
    *(struct aws_allocator **)&str->allocator = aws_default_allocator();
    *(size_t *)&str->len = len;
    ((uint8_t *)str->bytes)[len] = 0;
    return str;
}

void aws_string_eq_byte_buf_harness(void) {
    bool str_is_null;
    bool buf_is_null;

    const struct aws_string *str = NULL;
    struct aws_byte_buf buf;
    struct aws_byte_buf *buf_ptr = NULL;

    if (!str_is_null) {
        str = make_arbitrary_aws_string_nondet_len_with_max(MAX_BUFFER_SIZE);
        __CPROVER_assume(aws_string_is_valid(str));
    }

    if (!buf_is_null) {
        __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
        ensure_byte_buf_has_allocated_buffer_member(&buf);
        __CPROVER_assume(aws_byte_buf_is_valid(&buf));
        buf_ptr = &buf;
    }

    size_t old_str_len = (str != NULL) ? str->len : 0;
    size_t old_buf_len = (buf_ptr != NULL) ? buf_ptr->len : 0;
    size_t old_buf_capacity = (buf_ptr != NULL) ? buf_ptr->capacity : 0;
    struct aws_allocator *old_buf_allocator = (buf_ptr != NULL) ? buf_ptr->allocator : NULL;
    uint8_t *old_buf_buffer = (buf_ptr != NULL) ? buf_ptr->buffer : NULL;

    bool result = aws_string_eq_byte_buf(str, buf_ptr);

    if (str == NULL && buf_ptr == NULL) {
        assert(result == true);
    } else if (str == NULL || buf_ptr == NULL) {
        assert(result == false);
    } else {
        if (str->len != buf_ptr->len) {
            assert(result == false);
        }
        if (str->len == 0 && buf_ptr->len == 0) {
            assert(result == true);
        }
    }

    if (str != NULL) {
        assert(str->len == old_str_len);
    }

    if (buf_ptr != NULL) {
        assert(buf_ptr->len == old_buf_len);
        assert(buf_ptr->capacity == old_buf_capacity);
        assert(buf_ptr->allocator == old_buf_allocator);
        assert(buf_ptr->buffer == old_buf_buffer);
    }

    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }
    if (buf_ptr != NULL) {
        assert(aws_byte_buf_is_valid(buf_ptr));
    }
}
