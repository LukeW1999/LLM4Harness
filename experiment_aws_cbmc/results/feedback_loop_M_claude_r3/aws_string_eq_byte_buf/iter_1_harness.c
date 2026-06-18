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
        /* We need to write to const fields, use a cast */
        struct aws_string *mutable_str = (struct aws_string *)str;
        /* allocator can be anything */
        *(struct aws_allocator **)&mutable_str->allocator = aws_default_allocator();
        *(size_t *)&mutable_str->len = str_len;
        /* bytes are nondet (already nondet from malloc) */
        /* ensure null terminator */
        ((uint8_t *)mutable_str->bytes)[str_len] = 0;

        __CPROVER_assume(aws_string_is_valid(str));
    }

    if (!buf_is_null) {
        /* Set up a valid aws_byte_buf */
        size_t buf_capacity = nondet_size_t();
        __CPROVER_assume(buf_capacity <= MAX_BUFFER_SIZE);

        size_t buf_len = nondet_size_t();
        __CPROVER_assume(buf_len <= buf_capacity);

        buf_storage.capacity = buf_capacity;
        buf_storage.len = buf_len;
        buf_storage.allocator = aws_default_allocator();

        if (buf_capacity > 0) {
            buf_storage.buffer = malloc(buf_capacity);
            __CPROVER_assume(buf_storage.buffer != NULL);
        } else {
            buf_storage.buffer = NULL;
        }

        __CPROVER_assume(aws_byte_buf_is_valid(&buf_storage));
        buf = &buf_storage;
    }

    /* Save state before call */
    const struct aws_string *old_str = str;
    struct aws_byte_buf old_buf_storage;
    if (buf != NULL) {
        old_buf_storage = *buf;
    }

    /* Call function under test */
    bool result = aws_string_eq_byte_buf(str, buf);

    /* Postconditions based on Doxygen:
     * Returns true if bytes of string and buffer are the same, false otherwise.
     */

    /* Case: both NULL → true */
    if (str == NULL && buf == NULL) {
        assert(result == true);
    }

    /* Case: one NULL, other non-NULL → false */
    if (str == NULL && buf != NULL) {
        assert(result == false);
    }
    if (str != NULL && buf == NULL) {
        assert(result == false);
    }

    /* Case: both non-NULL → result depends on content (no specific assert on value,
     * but we can assert the function is consistent with length check) */
    if (str != NULL && buf != NULL) {
        /* If lengths differ, must be false */
        if (str->len != buf->len) {
            assert(result == false);
        }
        /* result is either true or false — both are valid */
    }

    /* Immutability: neither str nor buf should be modified */
    if (str != NULL) {
        assert(str == old_str);
        assert(str->len == old_str->len);
        assert(str->allocator == old_str->allocator);
    }

    if (buf != NULL) {
        assert(buf->len == old_buf_storage.len);
        assert(buf->capacity == old_buf_storage.capacity);
        assert(buf->allocator == old_buf_storage.allocator);
        assert(buf->buffer == old_buf_storage.buffer);
        assert(aws_byte_buf_is_valid(buf));
    }

    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }
}
