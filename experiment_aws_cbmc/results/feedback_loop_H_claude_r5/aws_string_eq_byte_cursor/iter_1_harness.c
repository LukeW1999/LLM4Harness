#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

/**
 * Harness for aws_string_eq_byte_cursor
 *
 * From the implementation:
 * - If both str and cur are NULL, returns true
 * - If exactly one is NULL, returns false
 * - Otherwise, returns aws_array_eq(str->bytes, str->len, cur->ptr, cur->len)
 *
 * The function is a pure predicate (no side effects):
 * 1. Changed fields: none (pure comparison)
 * 2. Unchanged fields: all fields of str and cur remain unchanged
 * 3. Failure path: N/A (returns bool, not error code)
 * 4. Validity invariants: str and cur remain valid after the call
 */

/* We need a helper to allocate an aws_string with bounded length */
#ifndef MAX_STRING_LEN
#define MAX_STRING_LEN 8
#endif

#ifndef MAX_CURSOR_LEN
#define MAX_CURSOR_LEN 8
#endif

struct aws_string *ensure_string_is_allocated_bounded(size_t max_len) {
    size_t len;
    __CPROVER_assume(len <= max_len);
    
    /* Allocate enough memory for the struct plus len+1 bytes (for null terminator) */
    struct aws_string *str = malloc(sizeof(struct aws_string) + len + 1);
    if (str == NULL) {
        return NULL;
    }
    /* Set the length field (const, so we use a trick) */
    *(size_t *)&str->len = len;
    /* allocator can be anything (including NULL for static strings) */
    *(struct aws_allocator **)&str->allocator = nondet_bool() ? aws_default_allocator() : NULL;
    /* bytes are already allocated as part of the struct */
    return str;
}

void aws_string_eq_byte_cursor_harness(void) {
    /* Non-deterministically choose whether str and cur are NULL */
    bool str_is_null = nondet_bool();
    bool cur_is_null = nondet_bool();

    const struct aws_string *str = NULL;
    struct aws_byte_cursor cur_val;
    const struct aws_byte_cursor *cur = NULL;

    if (!str_is_null) {
        str = ensure_string_is_allocated_bounded(MAX_STRING_LEN);
        __CPROVER_assume(str != NULL);
        __CPROVER_assume(aws_string_is_valid(str));
    }

    if (!cur_is_null) {
        __CPROVER_assume(aws_byte_cursor_is_bounded(&cur_val, MAX_CURSOR_LEN));
        ensure_byte_cursor_has_allocated_buffer_member(&cur_val);
        __CPROVER_assume(aws_byte_cursor_is_valid(&cur_val));
        cur = &cur_val;
    }

    /* Save old state for immutability checks */
    const struct aws_string *old_str = str;
    size_t old_str_len = (str != NULL) ? str->len : 0;
    struct aws_allocator *old_str_allocator = (str != NULL) ? str->allocator : NULL;

    struct aws_byte_cursor old_cur_val;
    size_t old_cur_len = 0;
    uint8_t *old_cur_ptr = NULL;
    if (cur != NULL) {
        old_cur_val = cur_val;
        old_cur_len = cur_val.len;
        old_cur_ptr = cur_val.ptr;
    }

    /* Call the function under test */
    bool result = aws_string_eq_byte_cursor(str, cur);

    /* Postcondition 1: Both NULL => true */
    if (str == NULL && cur == NULL) {
        assert(result == true);
    }

    /* Postcondition 2: Exactly one NULL => false */
    if (str == NULL && cur != NULL) {
        assert(result == false);
    }
    if (str != NULL && cur == NULL) {
        assert(result == false);
    }

    /* Postcondition 3: Neither NULL => result depends on content comparison */
    /* We can't easily assert the exact value here without reimplementing aws_array_eq,
     * but we can assert that if lengths differ, result must be false */
    if (str != NULL && cur != NULL) {
        if (str->len != cur->len) {
            assert(result == false);
        }
        /* If lengths are both 0, result must be true */
        if (str->len == 0 && cur->len == 0) {
            assert(result == true);
        }
    }

    /* Postcondition 4: str fields unchanged (immutability) */
    if (str != NULL) {
        assert(str == old_str);
        assert(str->len == old_str_len);
        assert(str->allocator == old_str_allocator);
    }

    /* Postcondition 5: cur fields unchanged (immutability) */
    if (cur != NULL) {
        assert(cur_val.len == old_cur_len);
        assert(cur_val.ptr == old_cur_ptr);
    }

    /* Postcondition 6: Validity invariants still hold */
    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }
    if (cur != NULL) {
        assert(aws_byte_cursor_is_valid(cur));
    }
}

void aws_string_eq_byte_cursor_harness(void) {
    aws_string_eq_byte_cursor_harness();
    return 0;
}
