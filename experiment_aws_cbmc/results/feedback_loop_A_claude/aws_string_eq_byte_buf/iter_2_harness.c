#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 4
#endif

void aws_string_eq_byte_buf_harness(void) {
    /* Non-deterministically decide whether str and buf are NULL */
    bool str_is_null;
    bool buf_is_null;

    /* Set up aws_string */
    struct aws_string *str = NULL;
    if (!str_is_null) {
        size_t str_len;
        __CPROVER_assume(str_len <= MAX_BUFFER_SIZE);
        str = malloc(sizeof(struct aws_string) + str_len + 1);
        __CPROVER_assume(str != NULL);
        *(size_t *)(&str->len) = str_len;
        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* Set up aws_byte_buf */
    struct aws_byte_buf buf_storage;
    struct aws_byte_buf *buf = NULL;
    if (!buf_is_null) {
        __CPROVER_assume(aws_byte_buf_is_bounded(&buf_storage, MAX_BUFFER_SIZE));
        ensure_byte_buf_has_allocated_buffer_member(&buf_storage);
        __CPROVER_assume(aws_byte_buf_is_valid(&buf_storage));
        buf = &buf_storage;
    }

    /* Save old state */
    size_t old_buf_len = 0;
    size_t old_buf_capacity = 0;
    uint8_t *old_buf_buffer = NULL;
    struct aws_allocator *old_buf_allocator = NULL;
    if (buf != NULL) {
        old_buf_len = buf->len;
        old_buf_capacity = buf->capacity;
        old_buf_buffer = buf->buffer;
        old_buf_allocator = buf->allocator;
    }

    size_t old_str_len = (str != NULL) ? str->len : 0;

    /* Call function under test */
    bool result = aws_string_eq_byte_buf(str, buf);

    /* Postcondition 1: Both NULL → true */
    if (str == NULL && buf == NULL) {
        assert(result == true);
    }

    /* Postcondition 2: str NULL, buf not NULL → false */
    if (str == NULL && buf != NULL) {
        assert(result == false);
    }

    /* Postcondition 3: str not NULL, buf NULL → false */
    if (str != NULL && buf == NULL) {
        assert(result == false);
    }

    /* Postcondition 4: If result is true and neither is NULL, lengths must match */
    if (str != NULL && buf != NULL && result == true) {
        assert(str->len == buf->len);
    }

    /* Postcondition 5: If lengths differ and neither is NULL, result must be false */
    if (str != NULL && buf != NULL && str->len != buf->len) {
        assert(result == false);
    }

    /* Postcondition 6: Input structures are not modified */
    if (buf != NULL) {
        assert(buf->len == old_buf_len);
        assert(buf->capacity == old_buf_capacity);
        assert(buf->buffer == old_buf_buffer);
        assert(buf->allocator == old_buf_allocator);
    }

    if (str != NULL) {
        assert(str->len == old_str_len);
    }

    /* Postcondition 7: Validity invariants still hold */
    if (buf != NULL) {
        assert(aws_byte_buf_is_valid(buf));
    }
    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }
}
