#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_string_eq_byte_buf_harness(void) {
    /* 1. Declare nondeterministic inputs */
    const struct aws_string *str;
    struct aws_byte_buf *buf;

    /* str may be NULL or point to a valid aws_string */
    if (nondet_bool()) {
        size_t str_len = nondet_size_t();
        __CPROVER_assume(str_len <= MAX_BUFFER_SIZE);
        struct aws_string *tmp = malloc(sizeof(struct aws_string) + str_len + 1);
        __CPROVER_assume(tmp != NULL);
        tmp->allocator = aws_default_allocator();
        tmp->len = str_len;
        for (size_t i = 0; i < str_len; ++i) {
            ((uint8_t *)tmp->bytes)[i] = nondet_uint8_t();
        }
        ((uint8_t *)tmp->bytes)[str_len] = 0; /* null‑terminator */
        str = tmp;
    } else {
        str = NULL;
    }

    /* buf may be NULL or point to a valid aws_byte_buf */
    if (nondet_bool()) {
        struct aws_byte_buf local_buf;
        buf = &local_buf;
        ensure_byte_buf_has_allocated_buffer_member(buf);
        __CPROVER_assume(aws_byte_buf_is_bounded(buf, MAX_BUFFER_SIZE));
        buf->capacity = nondet_size_t();
        __CPROVER_assume(buf->capacity <= MAX_BUFFER_SIZE);
        buf->len = nondet_size_t();
        __CPROVER_assume(buf->len <= buf->capacity);
        for (size_t i = 0; i < buf->len; ++i) {
            buf->buffer[i] = nondet_uint8_t();
        }
        buf->allocator = aws_default_allocator();
    } else {
        buf = NULL;
    }

    /* 2. Save old state for immutability checks */
    struct aws_allocator *old_str_allocator = NULL;
    size_t old_str_len = 0;
    struct store_byte_from_buffer old_str_bytes = {0};
    if (str != NULL) {
        old_str_allocator = str->allocator;
        old_str_len = str->len;
        save_byte_from_array(str->bytes, str->len, &old_str_bytes);
    }

    struct aws_byte_buf old_buf = {0};
    struct store_byte_from_buffer old_buf_bytes = {0};
    if (buf != NULL) {
        old_buf = *buf;
        save_byte_from_array(buf->buffer, buf->len, &old_buf_bytes);
    }

    /* 3. Call function under test */
    bool result = aws_string_eq_byte_buf(str, buf);

    /* 4. Postcondition: return value matches specification */
    if (str == NULL && buf == NULL) {
        assert(result == true);
    } else if (str == NULL || buf == NULL) {
        assert(result == false);
    } else {
        bool expected = aws_array_eq(str->bytes, str->len, buf->buffer, buf->len);
        assert(result == expected);
    }

    /* 5. Unchanged fields (immutability) */
    if (str != NULL) {
        assert(str->allocator == old_str_allocator);
        assert(str->len == old_str_len);
        assert_byte_from_buffer_matches(str->bytes, &old_str_bytes);
    }
    if (buf != NULL) {
        assert(buf->allocator == old_buf.allocator);
        assert(buf->capacity == old_buf.capacity);
        assert(buf->len == old_buf.len);
        assert(buf->buffer == old_buf.buffer);
        assert_byte_from_buffer_matches(buf->buffer, &old_buf_bytes);
    }

    /* 6. Validity invariants after the call */
    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }
    if (buf != NULL) {
        assert(aws_byte_buf_is_bounded(buf, MAX_BUFFER_SIZE));
    }
}
