#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>

void aws_string_eq_byte_buf_harness(void) {
    /* 1. Declare inputs */
    struct aws_string *str;
    struct aws_byte_buf *buf;

    /* nondet decide whether str is NULL */
    if (nondet_bool()) {
        str = NULL;
    } else {
        /* allocate a string with a nondet length bounded by MAX_BUFFER_SIZE */
        size_t str_len = nondet_size_t();
        __CPROVER_assume(str_len <= MAX_BUFFER_SIZE);

        /* allocate memory for the struct plus the flexible array member */
        str = malloc(sizeof(struct aws_string) + str_len);
        __CPROVER_assume(str != NULL);

        /* set fields (cast away constness for initialization) */
        str->allocator = aws_default_allocator();
        *((size_t *)&str->len) = str_len; /* const field */

        /* fill the bytes with nondet data */
        for (size_t i = 0; i < str_len; ++i) {
            ((uint8_t *)str->bytes)[i] = nondet_uint8_t();
        }

        /* assume the string is valid */
        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* nondet decide whether buf is NULL */
    if (nondet_bool()) {
        buf = NULL;
    } else {
        static struct aws_byte_buf local_buf;
        buf = &local_buf;

        /* bound the buffer */
        __CPROVER_assume(aws_byte_buf_is_bounded(buf, MAX_BUFFER_SIZE));

        /* allocate the internal buffer member */
        ensure_byte_buf_has_allocated_buffer_member(buf);

        /* set allocator */
        buf->allocator = aws_default_allocator();

        /* nondet length and capacity, respecting bounds */
        size_t buf_len = nondet_size_t();
        __CPROVER_assume(buf_len <= MAX_BUFFER_SIZE);
        buf->len = buf_len;

        size_t cap_extra = nondet_size_t();
        __CPROVER_assume(buf_len + cap_extra <= MAX_BUFFER_SIZE);
        buf->capacity = buf_len + cap_extra;

        buf->owns_buffer = true;
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
        assert(str->allocator == aws_default_allocator());
        assert(str->len == old_str->len);
        assert_byte_from_buffer_matches(str->bytes, &old_str_bytes);
    }

    if (buf != NULL) {
        assert(buf->allocator == aws_default_allocator());
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
