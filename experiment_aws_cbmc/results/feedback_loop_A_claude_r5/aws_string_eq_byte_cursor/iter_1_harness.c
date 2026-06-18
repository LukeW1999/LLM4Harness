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
    /* allocator can be NULL or non-NULL */
    *(struct aws_allocator **)&str->allocator = nondet_bool() ? NULL : aws_default_allocator();
    /* bytes are already allocated as part of the struct */
    return str;
}

void aws_string_eq_byte_cursor_harness(void) {
    /* 1. Declare inputs non-deterministically */
    
    /* str can be NULL or a valid aws_string */
    struct aws_string *str = NULL;
    bool str_is_null = nondet_bool();
    if (!str_is_null) {
        str = ensure_string_is_allocated_bounded(MAX_STRING_LEN);
        __CPROVER_assume(str != NULL);
        __CPROVER_assume(aws_string_is_valid(str));
    }
    
    /* cur can be NULL or a valid aws_byte_cursor */
    struct aws_byte_cursor *cur = NULL;
    bool cur_is_null = nondet_bool();
    if (!cur_is_null) {
        cur = malloc(sizeof(struct aws_byte_cursor));
        __CPROVER_assume(cur != NULL);
        __CPROVER_assume(aws_byte_cursor_is_bounded(cur, MAX_CURSOR_LEN));
        ensure_byte_cursor_has_allocated_buffer_member(cur);
        __CPROVER_assume(aws_byte_cursor_is_valid(cur));
    }
    
    /* 2. Save old state for immutability checks */
    struct aws_string str_old;
    struct aws_byte_cursor cur_old;
    
    if (str != NULL) {
        str_old = *str;
    }
    if (cur != NULL) {
        cur_old = *cur;
    }
    
    /* 3. Call function under test */
    bool result = aws_string_eq_byte_cursor(str, cur);
    
    /* 4. Assert postconditions */
    
    /* Both NULL → true */
    if (str == NULL && cur == NULL) {
        assert(result == true);
    }
    
    /* Exactly one NULL → false */
    if (str == NULL && cur != NULL) {
        assert(result == false);
    }
    if (str != NULL && cur == NULL) {
        assert(result == false);
    }
    
    /* Neither NULL: result depends on content comparison */
    if (str != NULL && cur != NULL) {
        /* If lengths differ, result must be false */
        if (str->len != cur->len) {
            assert(result == false);
        }
        /* If lengths are both 0, result must be true */
        if (str->len == 0 && cur->len == 0) {
            assert(result == true);
        }
    }
    
    /* 5. Assert unchanged fields (pure predicate, no side effects) */
    if (str != NULL) {
        assert(str->allocator == str_old.allocator);
        assert(str->len == str_old.len);
        /* Validity invariant still holds */
        assert(aws_string_is_valid(str));
    }
    
    if (cur != NULL) {
        assert(cur->ptr == cur_old.ptr);
        assert(cur->len == cur_old.len);
        /* Validity invariant still holds */
        assert(aws_byte_cursor_is_valid(cur));
    }
}
