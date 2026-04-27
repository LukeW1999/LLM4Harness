#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/proof_allocators.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/**
 * Harness for aws_string_eq_byte_cursor
 *
 * From Doxygen: Returns true if bytes of string and cursor are the same, false otherwise.
 *
 * From implementation:
 * - If both str and cur are NULL, returns true
 * - If exactly one is NULL, returns false
 * - Otherwise, returns aws_array_eq(str->bytes, str->len, cur->ptr, cur->len)
 *
 * Postconditions:
 * 1. Changed fields: only the return value (bool result)
 * 2. Unchanged fields: str and cur are not modified (immutability)
 * 3. Both return paths: result is true or false
 * 4. Validity invariants: str and cur remain valid after the call
 */

/* Helper to allocate a valid aws_string non-deterministically */
struct aws_string *ensure_string_is_allocated_nondet_length(void) {
    /* Use a bounded length for the string */
    size_t len;
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);
    
    /* Allocate memory for the string header + bytes + null terminator */
    struct aws_string *str = malloc(sizeof(struct aws_string) + len + 1);
    if (str == NULL) {
        return NULL;
    }
    
    /* Initialize the fields - need to cast away const for initialization */
    *(struct aws_allocator **)&str->allocator = nondet_bool() ? NULL : can_fail_allocator();
    *(size_t *)&str->len = len;
    /* bytes are non-deterministic (already set by malloc in CBMC) */
    /* Ensure null terminator */
    ((uint8_t *)str->bytes)[len] = 0;
    
    return str;
}

void aws_string_eq_byte_cursor_harness(void) {
    /* 1. Declare inputs non-deterministically */
    
    /* str can be NULL or a valid aws_string */
    struct aws_string *str = NULL;
    bool str_is_null = nondet_bool();
    if (!str_is_null) {
        str = ensure_string_is_allocated_nondet_length();
        __CPROVER_assume(str != NULL);
        __CPROVER_assume(aws_string_is_valid(str));
    }
    
    /* cur can be NULL or a valid aws_byte_cursor */
    struct aws_byte_cursor *cur = NULL;
    bool cur_is_null = nondet_bool();
    if (!cur_is_null) {
        cur = malloc(sizeof(struct aws_byte_cursor));
        __CPROVER_assume(cur != NULL);
        __CPROVER_assume(aws_byte_cursor_is_bounded(cur, MAX_BUFFER_SIZE));
        ensure_byte_cursor_has_allocated_buffer_member(cur);
        __CPROVER_assume(aws_byte_cursor_is_valid(cur));
    }
    
    /* 2. Save old state for immutability checks */
    /* Save string state */
    size_t old_str_len = (str != NULL) ? str->len : 0;
    const uint8_t *old_str_bytes = (str != NULL) ? str->bytes : NULL;
    struct aws_allocator *old_str_allocator = (str != NULL) ? str->allocator : NULL;
    
    /* Save cursor state */
    size_t old_cur_len = (cur != NULL) ? cur->len : 0;
    uint8_t *old_cur_ptr = (cur != NULL) ? cur->ptr : NULL;
    
    /* 3. Call function under test */
    bool result = aws_string_eq_byte_cursor(str, cur);
    
    /* 4. Assert postconditions */
    
    /* Case: both NULL -> must return true */
    if (str == NULL && cur == NULL) {
        assert(result == true);
    }
    /* Case: exactly one NULL -> must return false */
    else if (str == NULL || cur == NULL) {
        assert(result == false);
    }
    /* Case: neither NULL -> result depends on content comparison */
    else {
        /* If lengths differ, must return false */
        if (str->len != cur->len) {
            assert(result == false);
        }
        /* If lengths are both 0, must return true */
        if (str->len == 0 && cur->len == 0) {
            assert(result == true);
        }
        /* result is either true or false - both are valid */
        assert(result == true || result == false);
    }
    
    /* 5. Assert immutability - str fields unchanged */
    if (str != NULL) {
        assert(str->len == old_str_len);
        assert(str->bytes == old_str_bytes);
        assert(str->allocator == old_str_allocator);
    }
    
    /* 6. Assert immutability - cur fields unchanged */
    if (cur != NULL) {
        assert(cur->len == old_cur_len);
        assert(cur->ptr == old_cur_ptr);
    }
    
    /* 7. Assert validity invariants still hold */
    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }
    if (cur != NULL) {
        assert(aws_byte_cursor_is_valid(cur));
    }
}
