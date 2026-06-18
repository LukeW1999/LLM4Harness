#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 256

void aws_string_eq_byte_buf_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    /* 1. Declare inputs */
    struct aws_string *str;
    struct aws_byte_buf *buf = NULL;
    struct aws_byte_buf buf_obj; /* stack‑allocated buffer object */

    /* nondet decide whether str is NULL */
    if (nondet_bool()) {
        str = NULL;
    } else {
        size_t str_len = nondet_size_t();
        __CPROVER_assume(str_len <= MAX_BUFFER_SIZE);

        uint8_t str_buf[MAX_BUFFER_SIZE];
        for (size_t i = 0; i < str_len; ++i) {
            str_buf[i] = nondet_uint8_t();
        }

        str = aws_string_new_from_array(allocator, str_buf, str_len);
        __CPROVER_assume(str != NULL);
        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* nondet decide whether buf is NULL */
    if (nondet_bool()) {
        buf = NULL;
    } else {
        buf = &buf_obj;

        size_t cap = nondet_size_t();
        __CPROVER_assume(cap <= MAX_BUFFER_SIZE);
        aws_byte_buf_init(buf, allocator, cap);
        __CPROVER_assume(buf->buffer != NULL);

        size_t buf_len = nondet_size_t();
        __CPROVER_assume(buf_len <= cap);
        buf->len = buf_len;
        buf->owns_buffer = true;

        for (size_t i = 0; i < buf->capacity; ++i) {
            buf->buffer[i] = nondet_uint8_t();
        }

        __CPROVER_assume(aws_byte_buf_is_bounded(buf, MAX_BUFFER_SIZE));
    }

    /* 2. Save old state for immutability checks */
    struct aws_string *old_str = str;
    struct aws_byte_buf old_buf = {0};
    struct store_byte_from_buffer old_str_bytes = {0};
    struct store_byte_from_buffer old_buf_bytes = {0};

    if (str != NULL) {
        save_byte_from_array(str->bytes, str->len, &old_str_bytes);
    }
    if (buf != NULL) {
        old_buf = *buf;
        save_byte_from_array(buf->buffer, buf->len, &old_buf_bytes);
    }

    /* 3. Call function under test */
    bool result = aws_string_eq_byte_buf(str, buf);

    /* 4. Post‑condition on return value */
    if (str == NULL || buf == NULL) {
        /* According to implementation, any NULL argument yields false */
        assert(result == false);
    } else {
        bool expected = true;
        if (str->len != buf->len) {
            expected = false;
        } else {
            for (size_t i = 0; i < str->len; ++i) {
                if (str->bytes[i] != buf->buffer[i]) {
                    expected = false;
                    break;
                }
            }
        }
        assert(result == expected);
    }

    /* 5. Unchanged fields (immutability) */
    if (str != NULL) {
        assert(str->allocator == allocator);
        assert(str->len == old_str->len);
        assert_byte_from_buffer_matches(str->bytes, &old_str_bytes);
    }

    if (buf != NULL) {
        assert(buf->allocator == allocator);
        assert(buf->len == old_buf.len);
        assert(buf->capacity == old_buf.capacity);
        assert(buf->owns_buffer == old_buf.owns_buffer);
        assert_bytes_match(buf->buffer, old_buf.buffer, buf->len);
    }

    /* 6. Validity invariants */
    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }
    if (buf != NULL) {
        assert(aws_byte_buf_is_bounded(buf, MAX_BUFFER_SIZE));
    }
}
