#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_string_eq_byte_cursor_harness() {
    /* 1. Declare and bound data structures */
    struct aws_string *str = NULL;
    struct aws_byte_cursor *cur = NULL;

    /* Non-deterministically decide if str is NULL or allocated */
    bool str_is_null = nondet_bool();
    if (!str_is_null) {
        str = malloc(sizeof(struct aws_string));
        __CPROVER_assume(str != NULL);
        /* Allocate the string with a bounded length */
        str->len = nondet_size_t();
        __CPROVER_assume(str->len <= MAX_BUFFER_SIZE);
        /* Allocate bytes array (len + 1 for null terminator) */
        /* Since bytes is a flexible array member, we need to allocate extra space */
        /* We model this by ensuring the memory is readable */
        __CPROVER_assume(AWS_MEM_IS_READABLE(str->bytes, str->len + 1));
        /* Set allocator */
        str->allocator = aws_default_allocator();
        /* Ensure the string is valid */
        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* Non-deterministically decide if cur is NULL or allocated */
    bool cur_is_null = nondet_bool();
    if (!cur_is_null) {
        cur = malloc(sizeof(struct aws_byte_cursor));
        __CPROVER_assume(cur != NULL);
        cur->len = nondet_size_t();
        __CPROVER_assume(cur->len <= MAX_BUFFER_SIZE);
        if (cur->len > 0) {
            cur->ptr = malloc(cur->len);
            __CPROVER_assume(cur->ptr != NULL);
            __CPROVER_assume(AWS_MEM_IS_READABLE(cur->ptr, cur->len));
        } else {
            cur->ptr = NULL;
        }
        __CPROVER_assume(aws_byte_cursor_is_valid(cur));
    }

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_string *old_str = str;
    struct aws_byte_cursor *old_cur = cur;
    /* Save string state if not NULL */
    size_t old_str_len = (str != NULL) ? str->len : 0;
    struct aws_allocator *old_str_allocator = (str != NULL) ? str->allocator : NULL;
    /* Save cursor state if not NULL */
    size_t old_cur_len = (cur != NULL) ? cur->len : 0;
    uint8_t *old_cur_ptr = (cur != NULL) ? cur->ptr : NULL;

    /* 3. Call function under test */
    bool result = aws_string_eq_byte_cursor(str, cur);

    /* 4. Assert postconditions */

    /* The function is a pure predicate — it does not modify its arguments.
     * It returns true if both are NULL, or if both are non-NULL and their bytes match.
     * It returns false otherwise.
     */

    /* 5. Assert fields that must NOT change regardless of result */
    if (str != NULL) {
        assert(str->len == old_str_len);
        assert(str->allocator == old_str_allocator);
        /* bytes content should not change */
    }
    if (cur != NULL) {
        assert(cur->len == old_cur_len);
        assert(cur->ptr == old_cur_ptr);
    }

    /* 6. Assert validity invariants always hold */
    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }
    if (cur != NULL) {
        assert(aws_byte_cursor_is_valid(cur));
    }

    /* 7. Assert the return value matches the specification */
    if (str == NULL && cur == NULL) {
        assert(result == true);
    } else if (str == NULL || cur == NULL) {
        assert(result == false);
    } else {
        /* Both non-NULL: result should be true iff bytes match */
        if (str->len == cur->len) {
            /* If lengths match, check bytes */
            bool bytes_match = true;
            for (size_t i = 0; i < str->len; i++) {
                if (str->bytes[i] != cur->ptr[i]) {
                    bytes_match = false;
                    break;
                }
            }
            assert(result == bytes_match);
        } else {
            assert(result == false);
        }
    }

    /* 8. Free allocated memory */
    if (str != NULL) {
        free(str);
    }
    if (cur != NULL) {
        if (cur->ptr != NULL) {
            free(cur->ptr);
        }
        free(cur);
    }
}
