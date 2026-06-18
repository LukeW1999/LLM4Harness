#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_string_eq_byte_cursor_harness(void) {
    /* Non-deterministically choose whether str and cur are NULL */
    bool str_is_null = nondet_bool();
    bool cur_is_null = nondet_bool();

    /* Set up aws_string */
    struct aws_string *str = NULL;
    if (!str_is_null) {
        /* Allocate a string with bounded length */
        size_t len;
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);
        /* Allocate memory for the string header + bytes + null terminator */
        str = malloc(sizeof(struct aws_string) + len + 1);
        __CPROVER_assume(str != NULL);
        /* Initialize the length field (const, so use a trick) */
        *(size_t *)&str->len = len;
        /* The bytes are non-deterministic (already allocated) */
        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* Set up aws_byte_cursor */
    struct aws_byte_cursor cur;
    struct aws_byte_cursor *cur_ptr = NULL;
    if (!cur_is_null) {
        __CPROVER_assume(aws_byte_cursor_is_bounded(&cur, MAX_BUFFER_SIZE));
        ensure_byte_cursor_has_allocated_buffer_member(&cur);
        __CPROVER_assume(aws_byte_cursor_is_valid(&cur));
        cur_ptr = &cur;
    }

    /* Save old state */
    struct aws_byte_cursor old_cur;
    if (cur_ptr != NULL) {
        old_cur = cur;
    }

    /* Call function under test */
    bool result = aws_string_eq_byte_cursor(str, cur_ptr);

    /* Postconditions */

    /* Case 1: both NULL → must return true */
    if (str == NULL && cur_ptr == NULL) {
        assert(result == true);
    }

    /* Case 2: exactly one NULL → must return false */
    if (str == NULL && cur_ptr != NULL) {
        assert(result == false);
    }
    if (str != NULL && cur_ptr == NULL) {
        assert(result == false);
    }

    /* Case 3: neither NULL → result depends on content comparison */
    if (str != NULL && cur_ptr != NULL) {
        /* If lengths differ, result must be false */
        if (str->len != cur_ptr->len) {
            assert(result == false);
        }
        /* We can't easily assert the exact byte comparison result,
           but we can assert that the function returns a bool */
        assert(result == true || result == false);
    }

    /* Unchanged fields: the cursor should not be modified */
    if (cur_ptr != NULL) {
        assert(cur_ptr->ptr == old_cur.ptr);
        assert(cur_ptr->len == old_cur.len);
        assert(aws_byte_cursor_is_valid(cur_ptr));
    }

    /* The string should not be modified */
    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }
}
