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
        /* Create a valid aws_byte_buf */
        size_t buf_len = nondet_size_t();
        size_t buf_capacity = nondet_size_t();
        __CPROVER_assume(buf_len <= buf_capacity);
        __CPROVER_assume(buf_capacity <= MAX_BUFFER_SIZE);

        buf_storage.len = buf_len;
        buf_storage.capacity = buf_capacity;
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

    /* Save old state */
    size_t old_str_len = (str != NULL) ? str->len : 0;
    size_t old_buf_len = (buf != NULL) ? buf->len : 0;
    size_t old_buf_capacity = (buf != NULL) ? buf->capacity : 0;
    uint8_t *old_buf_buffer = (buf != NULL) ? buf->buffer : NULL;
    struct aws_allocator *old_buf_allocator = (buf != NULL) ? buf->allocator : NULL;

    /* Call function under test */
    bool result = aws_string_eq_byte_buf(str, buf);

    /* Postconditions */

    /* 1. If both NULL, result must be true */
    if (str == NULL && buf == NULL) {
        assert(result == true);
    }

    /* 2. If exactly one is NULL, result must be false */
    if (str == NULL && buf != NULL) {
        assert(result == false);
    }
    if (str != NULL && buf == NULL) {
        assert(result == false);
    }

    /* 3. If both non-NULL, result depends on content comparison */
    /* We can't easily assert the exact value without re-implementing,
       but we can assert that the function doesn't corrupt the inputs */

    /* 4. Unchanged fields: str must not be modified */
    if (str != NULL) {
        assert(str->len == old_str_len);
        assert(aws_string_is_valid(str));
    }

    /* 5. Unchanged fields: buf must not be modified */
    if (buf != NULL) {
        assert(buf->len == old_buf_len);
        assert(buf->capacity == old_buf_capacity);
        assert(buf->buffer == old_buf_buffer);
        assert(buf->allocator == old_buf_allocator);
        assert(aws_byte_buf_is_valid(buf));
    }

    /* 6. Result is a bool (0 or 1) */
    assert(result == true || result == false);
}
