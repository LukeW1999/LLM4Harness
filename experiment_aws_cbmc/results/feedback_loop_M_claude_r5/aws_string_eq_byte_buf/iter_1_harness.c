#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_string_eq_byte_buf_harness() {
    /* Case 1: both NULL */
    /* Case 2: str NULL, buf non-NULL */
    /* Case 3: str non-NULL, buf NULL */
    /* Case 4: both non-NULL */

    /* Non-deterministically choose whether str and buf are NULL */
    bool str_is_null = nondet_bool();
    bool buf_is_null = nondet_bool();

    struct aws_string *str = NULL;
    struct aws_byte_buf buf_storage;
    struct aws_byte_buf *buf = NULL;

    if (!str_is_null) {
        /* Allocate a valid aws_string */
        size_t str_len = nondet_size_t();
        __CPROVER_assume(str_len <= MAX_BUFFER_SIZE);

        /* Allocate memory for aws_string header + bytes + null terminator */
        str = malloc(sizeof(struct aws_string) + str_len + 1);
        __CPROVER_assume(str != NULL);

        /* Initialize the string fields */
        /* We need to write to const fields, use a non-const pointer */
        struct aws_string *mutable_str = (struct aws_string *)str;
        /* allocator can be NULL or non-NULL */
        *(struct aws_allocator **)&mutable_str->allocator = nondet_bool() ? NULL : aws_default_allocator();
        *(size_t *)&mutable_str->len = str_len;
        /* bytes are non-deterministic (already allocated) */
        /* Ensure the null terminator is set */
        ((uint8_t *)mutable_str->bytes)[str_len] = 0;

        __CPROVER_assume(aws_string_is_valid(str));
    }

    if (!buf_is_null) {
        /* Set up a valid aws_byte_buf */
        __CPROVER_assume(aws_byte_buf_is_bounded(&buf_storage, MAX_BUFFER_SIZE));
        ensure_byte_buf_has_allocated_buffer_member(&buf_storage);
        __CPROVER_assume(aws_byte_buf_is_valid(&buf_storage));
        buf = &buf_storage;
    }

    /* Call the function under test */
    bool result = aws_string_eq_byte_buf(str, buf);

    /* Postconditions based on Doxygen:
     * Returns true if bytes of string and buffer are the same, false otherwise.
     * Special cases: both NULL -> true, one NULL -> false
     */

    if (str == NULL && buf == NULL) {
        assert(result == true);
    } else if (str == NULL || buf == NULL) {
        assert(result == false);
    } else {
        /* Both non-NULL: result depends on content comparison */
        /* We can assert that if lengths differ, result must be false */
        if (str->len != buf->len) {
            assert(result == false);
        }
        /* If result is true, lengths must be equal */
        if (result == true) {
            assert(str->len == buf->len);
        }
    }

    /* Immutability: the function should not modify str or buf */
    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }
    if (buf != NULL) {
        assert(aws_byte_buf_is_valid(buf));
        /* buf fields unchanged */
        assert(buf->len == buf_storage.len);
        assert(buf->capacity == buf_storage.capacity);
        assert(buf->allocator == buf_storage.allocator);
        assert(buf->buffer == buf_storage.buffer);
    }
}
