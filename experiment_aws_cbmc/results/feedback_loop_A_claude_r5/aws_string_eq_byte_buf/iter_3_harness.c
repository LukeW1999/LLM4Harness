#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

#ifndef MAX_STRING_LEN
#define MAX_STRING_LEN 8
#endif

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 8
#endif

void harness(void) {
    /* Non-deterministically decide if str and buf are NULL */
    bool str_is_null;
    bool buf_is_null;

    const struct aws_string *str = NULL;
    struct aws_byte_buf buf;
    struct aws_byte_buf *buf_ptr = NULL;

    /* Set up str */
    if (!str_is_null) {
        str = make_arbitrary_aws_string_nondet_len_with_max(MAX_STRING_LEN);
        __CPROVER_assume(str != NULL);
        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* Set up buf */
    if (!buf_is_null) {
        __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
        ensure_byte_buf_has_allocated_buffer_member(&buf);
        __CPROVER_assume(aws_byte_buf_is_valid(&buf));
        buf_ptr = &buf;
    }

    /* Save old state */
    const struct aws_string *old_str = str;
    size_t old_str_len = str ? str->len : 0;

    struct aws_byte_buf old_buf;
    if (buf_ptr != NULL) {
        old_buf = buf;
    }

    /* Call function under test */
    bool result = aws_string_eq_byte_buf(str, buf_ptr);

    /* Postconditions */

    /* Case: both NULL → must return true */
    if (str == NULL && buf_ptr == NULL) {
        assert(result == true);
    }
    /* Case: exactly one NULL → must return false */
    else if (str == NULL || buf_ptr == NULL) {
        assert(result == false);
    }
    /* Case: neither NULL → result depends on content comparison */
    else {
        /* If lengths differ, must return false */
        if (str->len != buf.len) {
            assert(result == false);
        }
        /* If lengths are both 0, must return true */
        if (str->len == 0 && buf.len == 0) {
            assert(result == true);
        }
    }

    /* Immutability: str pointer unchanged */
    assert(str == old_str);

    /* Immutability: str fields unchanged (if not null) */
    if (str != NULL) {
        assert(str->len == old_str_len);
        assert(aws_string_is_valid(str));
    }

    /* Immutability: buf fields unchanged (if not null) */
    if (buf_ptr != NULL) {
        assert(buf.len == old_buf.len);
        assert(buf.capacity == old_buf.capacity);
        assert(buf.allocator == old_buf.allocator);
        assert(buf.buffer == old_buf.buffer);
        assert(aws_byte_buf_is_valid(buf_ptr));
    }
}
