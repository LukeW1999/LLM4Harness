#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>

void aws_string_eq_byte_buf_harness(void) {
    /* 1. Declare inputs (may be NULL) */
    struct aws_string *str;
    struct aws_byte_buf *buf;

    /* nondet decide if str is NULL */
    if (nondet_bool()) {
        str = NULL;
    } else {
        /* allocate a string with bounded length */
        size_t max_len = MAX_BUFFER_SIZE;
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= max_len);

        /* allocate memory for struct + flexible array */
        str = malloc(sizeof(struct aws_string) + len);
        __CPROVER_assume(str != NULL);

        /* allocator can be NULL (static string) or default allocator */
        str->allocator = NULL;               /* static string case */
        str->len = len;

        /* allocate and fill the bytes */
        if (len > 0) {
            /* bytes already part of allocation; just nondet fill */
            for (size_t i = 0; i < len; ++i) {
                ((uint8_t *)str->bytes)[i] = nondet_uint8_t();
            }
        }

        /* assume the string satisfies its validity predicate */
        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* nondet decide if buf is NULL */
    if (nondet_bool()) {
        buf = NULL;
    } else {
        /* allocate a byte buffer structure */
        buf = malloc(sizeof(struct aws_byte_buf));
        __CPROVER_assume(buf != NULL);

        /* bound the buffer */
        __CPROVER_assume(aws_byte_buf_is_bounded(buf, MAX_BUFFER_SIZE));
        ensure_byte_buf_has_allocated_buffer_member(buf);

        /* set allocator (may be NULL for static buffers) */
        buf->allocator = NULL;

        /* nondet length within allocated capacity */
        size_t cap = buf->capacity;
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= cap);
        buf->len = len;

        /* nondet fill the buffer contents */
        if (len > 0) {
            for (size_t i = 0; i < len; ++i) {
                buf->buffer[i] = nondet_uint8_t();
            }
        }
    }

    /* 2. Save old state for immutability checks */
    struct aws_string *old_str = str;
    struct aws_string old_str_copy;
    struct store_byte_from_buffer old_str_bytes;
    if (str != NULL) {
        old_str_copy = *str; /* shallow copy of header */
        save_byte_from_array(str->bytes, str->len, &old_str_bytes);
    }

    struct aws_byte_buf *old_buf = buf;
    struct aws_byte_buf old_buf_copy;
    struct store_byte_from_buffer old_buf_bytes;
    if (buf != NULL) {
        old_buf_copy = *buf;
        save_byte_from_array(buf->buffer, buf->len, &old_buf_bytes);
    }

    /* 3. Call function under test */
    bool result = aws_string_eq_byte_buf(str, buf);

    /* 4. Compute expected result according to specification */
    bool expected;
    if (str == NULL && buf == NULL) {
        expected = true;
    } else if (str == NULL || buf == NULL) {
        expected = false;
    } else {
        expected = aws_array_eq(str->bytes, str->len, buf->buffer, buf->len);
    }

    /* 5. Assert return value matches expectation */
    assert(result == expected);

    /* 6. Assert unchanged fields (frame conditions) */
    if (str != NULL) {
        assert(str->allocator == old_str_copy.allocator);
        assert(str->len == old_str_copy.len);
        assert(str->bytes == old_str_copy.bytes);
        assert_byte_from_buffer_matches(str->bytes, &old_str_bytes);
    } else {
        assert(old_str == NULL);
    }

    if (buf != NULL) {
        assert(buf->allocator == old_buf_copy.allocator);
        assert(buf->capacity == old_buf_copy.capacity);
        assert(buf->len == old_buf_copy.len);
        assert(buf->buffer == old_buf_copy.buffer);
        assert_byte_from_buffer_matches(buf->buffer, &old_buf_bytes);
    } else {
        assert(old_buf == NULL);
    }

    /* 7. Assert validity invariants after the call */
    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }
    if (buf != NULL) {
        assert(aws_byte_buf_is_bounded(buf, MAX_BUFFER_SIZE));
    }

    /* 8. Clean up */
    if (str != NULL) {
        free(str);
    }
    if (buf != NULL) {
        free(buf);
    }
}
