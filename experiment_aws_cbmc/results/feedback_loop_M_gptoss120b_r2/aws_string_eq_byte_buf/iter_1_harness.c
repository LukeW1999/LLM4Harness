#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <aws/common/array_list.h>
#include <aws/common/linked_list.h>
#include <aws/common/math.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_string_eq_byte_buf_harness(void) {
    /* 1. Nondeterministic inputs, possibly NULL */
    struct aws_string *str;
    struct aws_byte_buf *buf;

    /* ----- aws_string ----- */
    if (nondet_bool()) {
        str = NULL;
    } else {
        size_t str_len = nondet_size_t();
        __CPROVER_assume(str_len <= MAX_BUFFER_SIZE);

        /* allocate struct + flexible array */
        str = malloc(sizeof(struct aws_string) + (str_len > 0 ? str_len - 1 : 0));
        __CPROVER_assume(str != NULL);

        /* set const fields via casts */
        *((struct aws_allocator **)&str->allocator) = aws_default_allocator();
        *((size_t *)&str->len) = str_len;

        /* fill bytes with nondet data */
        for (size_t i = 0; i < str_len; ++i) {
            ((uint8_t *)str->bytes)[i] = nondet_uint8_t();
        }

        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* ----- aws_byte_buf ----- */
    if (nondet_bool()) {
        buf = NULL;
    } else {
        buf = malloc(sizeof(struct aws_byte_buf));
        __CPROVER_assume(buf != NULL);

        *((struct aws_allocator **)&buf->allocator) = aws_default_allocator();

        size_t capacity = nondet_size_t();
        __CPROVER_assume(capacity <= MAX_BUFFER_SIZE);
        buf->capacity = capacity;

        size_t len = nondet_size_t();
        __CPROVER_assume(len <= capacity);
        buf->len = len;

        buf->buffer = malloc(capacity);
        __CPROVER_assume(buf->buffer != NULL);

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
        expected = aws_array_eq(str->bytes, str->len, buf->buffer, buf->len);
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
