#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 256

void aws_string_eq_byte_buf_harness(void) {
    /* 1. Nondeterministic inputs, possibly NULL */
    struct aws_string *str;
    struct aws_byte_buf *buf;

    /* ----- aws_string ----- */
    struct {
        struct aws_string s;
        uint8_t data[MAX_BUFFER_SIZE];
    } str_wrapper;

    if (nondet_bool()) {
        str = NULL;
    } else {
        size_t str_len = nondet_size_t();
        __CPROVER_assume(str_len <= MAX_BUFFER_SIZE);

        str = &str_wrapper.s;
        str->allocator = aws_default_allocator();
        str->len = str_len;

        for (size_t i = 0; i < str_len; ++i) {
            str->bytes[i] = nondet_uint8_t();
        }

        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* ----- aws_byte_buf ----- */
    static uint8_t buffer_array[MAX_BUFFER_SIZE];
    struct aws_byte_buf buf_local;

    if (nondet_bool()) {
        buf = NULL;
    } else {
        buf = &buf_local;

        buf->allocator = aws_default_allocator();

        size_t capacity = nondet_size_t();
        __CPROVER_assume(capacity <= MAX_BUFFER_SIZE);
        buf->capacity = capacity;

        size_t len = nondet_size_t();
        __CPROVER_assume(len <= capacity);
        buf->len = len;

        buf->buffer = buffer_array;

        for (size_t i = 0; i < len; ++i) {
            buf->buffer[i] = nondet_uint8_t();
        }

        buf->owns_buffer = true;

        __CPROVER_assume(aws_byte_buf_is_bounded(buf, MAX_BUFFER_SIZE));
    }

    /* 2. Save old state */
    struct aws_allocator *old_str_allocator = NULL;
    size_t old_str_len = 0;
    struct store_byte_from_buffer str_store;
    if (str != NULL) {
        old_str_allocator = str->allocator;
        old_str_len = str->len;
        save_byte_from_array(str->bytes, str->len, &str_store);
    }

    struct aws_byte_buf old_buf = {0};
    struct store_byte_from_buffer buf_store;
    if (buf != NULL) {
        old_buf = *buf;
        save_byte_from_array(buf->buffer, buf->len, &buf_store);
    }

    /* 3. Call function under test */
    bool result = aws_string_eq_byte_buf(str, buf);

    /* 4. Post‑condition: result matches specification */
    bool expected;
    if (str == NULL && buf == NULL) {
        expected = true;
    } else if (str == NULL || buf == NULL) {
        expected = false;
    } else {
        if (str->len != buf->len) {
            expected = false;
        } else {
            expected = true;
            for (size_t i = 0; i < str->len; ++i) {
                if (str->bytes[i] != buf->buffer[i]) {
                    expected = false;
                    break;
                }
            }
        }
    }
    assert(result == expected);

    /* 5. Unchanged fields */
    if (str != NULL) {
        assert(str->allocator == old_str_allocator);
        assert(str->len == old_str_len);
        assert_byte_from_buffer_matches(str->bytes, &str_store);
    }

    if (buf != NULL) {
        assert(buf->allocator == old_buf.allocator);
        assert(buf->capacity == old_buf.capacity);
        assert(buf->len == old_buf.len);
        assert(buf->buffer == old_buf.buffer);
        assert(buf->owns_buffer == old_buf.owns_buffer);
        assert_byte_from_buffer_matches(buf->buffer, &buf_store);
    }

    /* 6. Validity invariants */
    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }
    if (buf != NULL) {
        assert(aws_byte_buf_is_bounded(buf, MAX_BUFFER_SIZE));
    }
}
