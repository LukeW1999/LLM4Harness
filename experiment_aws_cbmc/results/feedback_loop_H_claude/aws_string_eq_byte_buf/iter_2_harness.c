#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 8
#endif

void aws_string_eq_byte_buf_harness(void) {
    /* Non-deterministically choose whether str and buf are NULL */
    bool str_is_null;
    bool buf_is_null;

    /* Set up aws_string */
    struct aws_string *str = NULL;
    if (!str_is_null) {
        /* Allocate a string with bounded length */
        size_t str_len;
        __CPROVER_assume(str_len <= MAX_BUFFER_SIZE);
        
        /* Allocate memory for the string header + bytes + null terminator */
        str = malloc(sizeof(struct aws_string) + str_len + 1);
        __CPROVER_assume(str != NULL);
        
        /* Initialize the string fields (cast away const for initialization) */
        *(size_t *)&str->len = str_len;
        /* ensure null terminator */
        ((uint8_t *)str->bytes)[str_len] = 0;
        
        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* Set up aws_byte_buf */
    struct aws_byte_buf buf;
    struct aws_byte_buf *buf_ptr = NULL;
    if (!buf_is_null) {
        __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
        ensure_byte_buf_has_allocated_buffer_member(&buf);
        __CPROVER_assume(aws_byte_buf_is_valid(&buf));
        buf_ptr = &buf;
    }

    /* Save old state */
    struct aws_byte_buf old_buf;
    if (buf_ptr != NULL) {
        old_buf = buf;
    }

    /* Call function under test */
    bool result = aws_string_eq_byte_buf(str, buf_ptr);

    /* Postconditions */

    /* Case 1: both NULL */
    if (str == NULL && buf_ptr == NULL) {
        assert(result == true);
    }
    /* Case 2: one is NULL, other is not */
    else if (str == NULL || buf_ptr == NULL) {
        assert(result == false);
    }
    /* Case 3: neither is NULL - result depends on content comparison */
    else {
        /* If lengths differ, must be false */
        if (str->len != buf_ptr->len) {
            assert(result == false);
        }
        /* If lengths are both 0, must be true */
        if (str->len == 0 && buf_ptr->len == 0) {
            assert(result == true);
        }
    }

    /* Unchanged fields: the function is a pure comparison, nothing should change */
    if (buf_ptr != NULL) {
        assert(buf_ptr->len == old_buf.len);
        assert(buf_ptr->capacity == old_buf.capacity);
        assert(buf_ptr->allocator == old_buf.allocator);
        assert(buf_ptr->buffer == old_buf.buffer);
        assert(aws_byte_buf_is_valid(buf_ptr));
    }

    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }
}
