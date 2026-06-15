#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
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
        /* allocate a string with a nondeterministic length */
        size_t str_len = nondet_size_t();
        __CPROVER_assume(str_len <= MAX_BUFFER_SIZE);
        struct aws_string *tmp = malloc(sizeof(struct aws_string) + str_len);
        __CPROVER_assume(tmp != NULL);
        /* allocator can be NULL (static string) or nondet */
        tmp->allocator = nondet_bool() ? NULL : (struct aws_allocator *)malloc(1);
        tmp->len = str_len;
        /* fill bytes with nondet data */
        for (size_t i = 0; i < str_len; ++i) {
            ((uint8_t *)tmp->bytes)[i] = nondet_uint8_t();
        }
        /* ensure the string satisfies its validity predicate */
        __CPROVER_assume(aws_string_is_valid(tmp));
        str = tmp;
    } else {
        str = NULL;
    }

    /* buf may be NULL or point to a valid aws_byte_buf */
    if (nondet_bool()) {
        buf = malloc(sizeof(struct aws_byte_buf));
        __CPROVER_assume(buf != NULL);
        ensure_byte_buf_has_allocated_buffer_member(buf);
        /* bound the buffer */
        __CPROVER_assume(aws_byte_buf_is_bounded(buf, MAX_BUFFER_SIZE));
        /* nondet capacity and length */
        buf->capacity = nondet_size_t();
        __CPROVER_assume(buf->capacity <= MAX_BUFFER_SIZE);
        buf->len = nondet_size_t();
        __CPROVER_assume(buf->len <= buf->capacity);
        /* fill the buffer with nondet data */
        for (size_t i = 0; i < buf->len; ++i) {
            buf->buffer[i] = nondet_uint8_t();
        }
        /* allocator can be NULL or nondet */
        buf->allocator = nondet_bool() ? NULL : (struct aws_allocator *)malloc(1);
    } else {
        buf = NULL;
    }

    /* 2. Save old state for immutability checks */
    struct aws_string old_str;
    struct store_byte_from_buffer old_str_bytes = {0};
    if (str != NULL) {
        old_str = *str;
        save_byte_from_array(str->bytes, str->len, &old_str_bytes);
    }

    struct aws_byte_buf old_buf;
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
        /* both non‑NULL: result must equal array equality */
        bool expected = aws_array_eq(str->bytes, str->len, buf->buffer, buf->len);
        assert(result == expected);
    }

    /* 5. Unchanged fields (immutability) */
    if (str != NULL) {
        assert(str->allocator == old_str.allocator);
        assert(str->len == old_str.len);
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
