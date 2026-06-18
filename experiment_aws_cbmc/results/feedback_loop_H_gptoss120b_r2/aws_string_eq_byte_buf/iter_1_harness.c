#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <aws/common/array.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>

void aws_string_eq_byte_buf_harness() {
    /* 1. Declare pointers that may be NULL */
    struct aws_string *str = NULL;
    struct aws_byte_buf *buf = NULL;

    /* nondeterministically decide if they are NULL */
    if (nondet_bool()) {
        /* allocate and initialize a valid aws_string */
        size_t str_len = nondet_size_t();
        __CPROVER_assume(str_len <= MAX_BUFFER_SIZE);

        /* allocate enough memory for flexible array member */
        str = malloc(sizeof(struct aws_string) + (str_len == 0 ? 0 : str_len - 1));
        __CPROVER_assume(str != NULL);

        str->allocator = aws_default_allocator();
        /* const fields can be written here for harness purposes */
        *((size_t *)&str->len) = str_len;   /* cast away const */
        for (size_t i = 0; i < str_len; ++i) {
            str->bytes[i] = nondet_uint8_t();
        }

        __CPROVER_assume(aws_string_is_valid(str));
    }

    if (nondet_bool()) {
        /* allocate and initialize a valid aws_byte_buf */
        buf = malloc(sizeof(struct aws_byte_buf));
        __CPROVER_assume(buf != NULL);

        buf->allocator = aws_default_allocator();

        size_t capacity = nondet_size_t();
        __CPROVER_assume(capacity <= MAX_BUFFER_SIZE);
        ensure_byte_buf_has_allocated_buffer_member(buf);
        buf->capacity = capacity;
        buf->len = nondet_size_t();
        __CPROVER_assume(buf->len <= buf->capacity);
        for (size_t i = 0; i < buf->capacity; ++i) {
            buf->buffer[i] = nondet_uint8_t();
        }

        __CPROVER_assume(aws_byte_buf_is_bounded(buf, MAX_BUFFER_SIZE));
    }

    /* 2. Save old state for immutability checks */
    struct aws_string old_str;
    struct store_byte_from_buffer old_str_bytes = {0};
    if (str != NULL) {
        old_str = *str; /* copies allocator and len (bytes not copied) */
        save_byte_from_array(str->bytes, str->len, &old_str_bytes);
    }

    struct aws_byte_buf old_buf;
    struct store_byte_from_buffer old_buf_bytes = {0};
    if (buf != NULL) {
        old_buf = *buf; /* copies allocator, buffer pointer, len, capacity */
        save_byte_from_array(buf->buffer, buf->capacity, &old_buf_bytes);
    }

    /* 3. Call function under test */
    bool result = aws_string_eq_byte_buf(str, buf);

    /* 4. Assert postconditions */
    if (str == NULL && buf == NULL) {
        assert(result == true);
    } else if (str == NULL || buf == NULL) {
        assert(result == false);
    } else {
        /* both non‑NULL: result must equal array equality */
        assert(result == aws_array_eq(str->bytes, str->len, buf->buffer, buf->len));
    }

    /* 5. Assert unchanged fields */
    if (str != NULL) {
        assert(str->allocator == old_str.allocator);
        assert(str->len == old_str.len);
        assert_bytes_match(str->bytes, old_str_bytes.buffer, str->len);
    }
    if (buf != NULL) {
        assert(buf->allocator == old_buf.allocator);
        assert(buf->capacity == old_buf.capacity);
        assert(buf->len == old_buf.len);
        assert(buf->buffer == old_buf.buffer);
        assert_bytes_match(buf->buffer, old_buf_bytes.buffer, buf->capacity);
    }

    /* 6. Assert validity invariants */
    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }
    if (buf != NULL) {
        assert(aws_byte_buf_is_bounded(buf, MAX_BUFFER_SIZE));
    }

    /* free allocated memory */
    free(str);
    free(buf);
}
