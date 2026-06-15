#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_string_eq_byte_buf_harness() {
    /* 1. Declare possibly NULL inputs */
    struct aws_string *str;
    struct aws_byte_buf *buf;

    /* nondeterministically decide if they are NULL */
    if (nondet_bool()) {
        str = NULL;
    } else {
        /* allocate space for the struct plus a bounded flexible array */
        size_t max_len = MAX_BUFFER_SIZE;
        str = malloc(sizeof(struct aws_string) + max_len);
        __CPROVER_assume(str != NULL);
        /* initialise fields */
        str->allocator = NULL; /* allocator not used by the function */
        str->len = nondet_size_t();
        __CPROVER_assume(str->len <= max_len);
        /* make the string appear valid */
        __CPROVER_assume(aws_string_is_valid(str));
    }

    if (nondet_bool()) {
        buf = NULL;
    } else {
        buf = malloc(sizeof(struct aws_byte_buf));
        __CPROVER_assume(buf != NULL);
        /* ensure the buffer member itself is allocated */
        ensure_byte_buf_has_allocated_buffer_member(buf);
        /* bound the whole buffer structure */
        __CPROVER_assume(aws_byte_buf_is_bounded(buf, MAX_BUFFER_SIZE));
        /* reasonable length vs capacity */
        __CPROVER_assume(buf->len <= buf->capacity);
    }

    /* 2. Save old state for immutability checks */
    struct aws_string *old_str = str ? malloc(sizeof(struct aws_string) + str->len) : NULL;
    struct aws_byte_buf old_buf;
    struct store_byte_from_buffer str_bytes_store;
    struct store_byte_from_buffer buf_bytes_store;

    if (str) {
        memcpy(old_str, str, sizeof(struct aws_string) + str->len);
        save_byte_from_array(str->bytes, str->len, &str_bytes_store);
    }

    if (buf) {
        old_buf = *buf;
        if (buf->buffer != NULL && buf->len > 0) {
            save_byte_from_array(buf->buffer, buf->len, &buf_bytes_store);
        }
    }

    /* 3. Call function under test */
    bool result = aws_string_eq_byte_buf(str, buf);

    /* 4. No fields are modified by the function – assert unchanged */
    if (str) {
        assert(str->allocator == old_str->allocator);
        assert(str->len == old_str->len);
        assert_byte_from_buffer_matches(str->bytes, &str_bytes_store);
        /* validity must still hold */
        assert(aws_string_is_valid(str));
    } else {
        assert(old_str == NULL);
    }

    if (buf) {
        assert(buf->allocator == old_buf.allocator);
        assert(buf->capacity == old_buf.capacity);
        assert(buf->len == old_buf.len);
        assert(buf->buffer == old_buf.buffer);
        if (buf->buffer != NULL && buf->len > 0) {
            assert_byte_from_buffer_matches(buf->buffer, &buf_bytes_store);
        }
        /* validity must still hold */
        assert(aws_byte_buf_is_bounded(buf, MAX_BUFFER_SIZE));
    } else {
        /* nothing to check for a NULL buffer */
    }

    /* 5. Result is a boolean – no further post‑condition needed */
    (void)result; /* silence unused‑variable warnings */
}
