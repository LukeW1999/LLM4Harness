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

    /* Non-deterministically choose which case to test */
    bool str_is_null = nondet_bool();
    bool buf_is_null = nondet_bool();

    struct aws_string *str = NULL;
    struct aws_byte_buf *buf = NULL;

    /* Set up str if non-null */
    struct aws_string *str_storage = NULL;
    if (!str_is_null) {
        /* Allocate an aws_string with bounded length */
        size_t str_len = nondet_size_t();
        __CPROVER_assume(str_len <= MAX_BUFFER_SIZE);

        /* Allocate memory for aws_string header + bytes + null terminator */
        str_storage = malloc(sizeof(struct aws_string) + str_len + 1);
        __CPROVER_assume(str_storage != NULL);

        /* Initialize the string fields - need to cast away const */
        *(struct aws_allocator **)&str_storage->allocator = aws_default_allocator();
        *(size_t *)&str_storage->len = str_len;
        /* bytes are non-deterministic (already allocated) */

        __CPROVER_assume(aws_string_is_valid(str_storage));
        str = str_storage;
    }

    /* Set up buf if non-null */
    struct aws_byte_buf buf_storage;
    if (!buf_is_null) {
        __CPROVER_assume(aws_byte_buf_is_bounded(&buf_storage, MAX_BUFFER_SIZE));
        ensure_byte_buf_has_allocated_buffer_member(&buf_storage);
        __CPROVER_assume(aws_byte_buf_is_valid(&buf_storage));
        buf = &buf_storage;
    }

    /* Save old state */
    struct aws_string *old_str = str;
    struct aws_byte_buf old_buf_val;
    if (buf != NULL) {
        old_buf_val = *buf;
    }

    /* Call function under test */
    bool result = aws_string_eq_byte_buf(str, buf);

    /* Postconditions */

    /* Case: both NULL → must return true */
    if (str == NULL && buf == NULL) {
        assert(result == true);
    }

    /* Case: exactly one NULL → must return false */
    if (str == NULL && buf != NULL) {
        assert(result == false);
    }
    if (str != NULL && buf == NULL) {
        assert(result == false);
    }

    /* Case: both non-NULL → result depends on content comparison */
    if (str != NULL && buf != NULL) {
        /* Result is either true or false — no specific assertion on value,
         * but we can assert the structures are unchanged */
        assert(str == old_str);
        assert(str->len == old_str->len);
        assert(str->allocator == old_str->allocator);

        assert(buf->len == old_buf_val.len);
        assert(buf->capacity == old_buf_val.capacity);
        assert(buf->buffer == old_buf_val.buffer);
        assert(buf->allocator == old_buf_val.allocator);

        /* Validity invariants still hold */
        assert(aws_string_is_valid(str));
        assert(aws_byte_buf_is_valid(buf));
    }

    /* Str pointer unchanged */
    assert(str == old_str);

    /* If buf was non-null, its fields are unchanged */
    if (buf != NULL) {
        assert(buf->len == old_buf_val.len);
        assert(buf->capacity == old_buf_val.capacity);
        assert(buf->buffer == old_buf_val.buffer);
        assert(buf->allocator == old_buf_val.allocator);
        assert(aws_byte_buf_is_valid(buf));
    }

    /* If str was non-null, its fields are unchanged */
    if (str != NULL) {
        assert(str->len == old_str->len);
        assert(str->allocator == old_str->allocator);
        assert(aws_string_is_valid(str));
    }
}
