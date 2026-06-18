#define MAX_BUFFER_SIZE 256

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_string_eq_byte_buf_harness(void) {
    uint8_t str_storage[sizeof(struct aws_string) + MAX_BUFFER_SIZE];
    struct aws_string *str = NULL;
    if (nondet_bool()) {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);
        str = (struct aws_string *)str_storage;
        str->allocator = aws_default_allocator();
        str->len = len;
        __CPROVER_assume(str->allocator != NULL);
    }

    struct aws_byte_buf buf_obj;
    struct aws_byte_buf *buf_ptr = NULL;
    if (nondet_bool()) {
        buf_obj.buffer = NULL;
        buf_obj.len = 0;
        buf_obj.capacity = 0;
        buf_obj.allocator = NULL;

        buf_ptr = &buf_obj;
        struct aws_allocator *allocator = aws_default_allocator();
        buf_ptr->allocator = allocator;
        __CPROVER_assume(buf_ptr->allocator != NULL);

        ensure_byte_buf_has_allocated_buffer_member(buf_ptr);
        __CPROVER_assume(buf_ptr->capacity <= MAX_BUFFER_SIZE);
        __CPROVER_assume(buf_ptr->len <= buf_ptr->capacity);
    }

    struct aws_string *old_str = str;
    struct store_byte_from_buffer old_str_bytes;
    if (str != NULL) {
        save_byte_from_array(str->bytes, str->len, &old_str_bytes);
    }

    struct aws_byte_buf old_buf;
    struct store_byte_from_buffer old_buf_bytes;
    if (buf_ptr != NULL) {
        old_buf = *buf_ptr;
        save_byte_from_array(buf_ptr->buffer, buf_ptr->len, &old_buf_bytes);
    }

    bool result = aws_string_eq_byte_buf(str, buf_ptr);

    bool expected;
    if (str == NULL && buf_ptr == NULL) {
        expected = true;
    } else if (str == NULL || buf_ptr == NULL) {
        expected = false;
    } else {
        expected = true;
        if (str->len != buf_ptr->len) {
            expected = false;
        } else {
            for (size_t i = 0; i < str->len; ++i) {
                if (str->bytes[i] != buf_ptr->buffer[i]) {
                    expected = false;
                    break;
                }
            }
        }
    }
    assert(result == expected);

    if (str != NULL) {
        assert(str->allocator == old_str->allocator);
        assert(str->len == old_str->len);
        assert_byte_from_buffer_matches(str->bytes, &old_str_bytes);
    }
    if (buf_ptr != NULL) {
        assert(buf_ptr->allocator == old_buf.allocator);
        assert(buf_ptr->capacity == old_buf.capacity);
        assert(buf_ptr->len == old_buf.len);
        assert_byte_from_buffer_matches(buf_ptr->buffer, &old_buf_bytes);
    }

    if (str != NULL) {
        assert(str->allocator != NULL);
        assert(str->len <= MAX_BUFFER_SIZE);
    }
    if (buf_ptr != NULL) {
        assert(buf_ptr->allocator != NULL);
        assert(buf_ptr->capacity <= MAX_BUFFER_SIZE);
        assert(buf_ptr->len <= buf_ptr->capacity);
    }
}
