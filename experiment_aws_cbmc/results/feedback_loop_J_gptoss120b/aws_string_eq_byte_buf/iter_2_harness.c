#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdbool.h>

void aws_string_eq_byte_buf_harness(void) {
    /* 1. Declare inputs (may be NULL) */
    struct aws_string *str;
    struct aws_byte_buf *buf;

    /* nondet choice for str being NULL */
    if (nondet_bool()) {
        str = NULL;
    } else {
        size_t max_len = MAX_BUFFER_SIZE;
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= max_len);

        /* allocate struct with flexible array member */
        str = malloc(sizeof(struct aws_string) + len);
        __CPROVER_assume(str != NULL);
        str->allocator = aws_default_allocator();
        str->len = len;

        /* assume the string is valid (bytes are readable) */
        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* nondet choice for buf being NULL */
    if (nondet_bool()) {
        buf = NULL;
    } else {
        buf = malloc(sizeof(struct aws_byte_buf));
        __CPROVER_assume(buf != NULL);
        ensure_byte_buf_has_allocated_buffer_member(buf);
        __CPROVER_assume(aws_byte_buf_is_bounded(buf, MAX_BUFFER_SIZE));
        __CPROVER_assume(aws_byte_buf_is_valid(buf));
    }

    /* 2. Save old state for immutability checks */
    struct aws_string *old_str = NULL;
    struct store_byte_from_buffer old_str_bytes = {0};
    if (str != NULL) {
        old_str = malloc(sizeof(struct aws_string));
        __CPROVER_assume(old_str != NULL);
        *old_str = *str; /* shallow copy of fields */
        save_byte_from_array(str->bytes, str->len, &old_str_bytes);
    }

    struct aws_byte_buf old_buf = {0};
    struct store_byte_from_buffer old_buf_bytes = {0};
    if (buf != NULL) {
        old_buf = *buf; /* shallow copy of fields */
        save_byte_from_array(buf->buffer, buf->len, &old_buf_bytes);
    }

    /* 3. Call function under test */
    bool result = aws_string_eq_byte_buf(str, buf);

    /* 4. Assert return value matches specification */
    if (str == NULL && buf == NULL) {
        assert(result == true);
    } else if (str == NULL || buf == NULL) {
        assert(result == false);
    } else {
        bool expected = aws_array_eq(str->bytes, str->len, buf->buffer, buf->len);
        assert(result == expected);
    }

    /* 5. Assert unchanged fields (function is pure) */
    if (str != NULL) {
        assert(str->allocator == old_str->allocator);
        assert(str->len == old_str->len);
        assert_bytes_match(str->bytes, old_str_bytes.buffer, str->len);
    }

    if (buf != NULL) {
        assert(buf->allocator == old_buf.allocator);
        assert(buf->capacity == old_buf.capacity);
        assert(buf->len == old_buf.len);
        assert(buf->buffer == old_buf.buffer);
        assert_bytes_match(buf->buffer, old_buf_bytes.buffer, buf->len);
    }

    /* 6. Assert validity invariants still hold */
    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }
    if (buf != NULL) {
        assert(aws_byte_buf_is_valid(buf));
    }
}
