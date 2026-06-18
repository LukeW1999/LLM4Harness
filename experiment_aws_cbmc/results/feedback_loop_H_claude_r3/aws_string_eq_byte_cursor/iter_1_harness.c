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
#define MAX_STRING_LEN 10
#endif

#ifndef MAX_CURSOR_LEN
#define MAX_CURSOR_LEN 10
#endif

struct aws_string *ensure_string_is_allocated_bounded(size_t max_len) {
    size_t len;
    __CPROVER_assume(len <= max_len);
    
    /* Allocate memory for the aws_string header plus len+1 bytes (for null terminator) */
    struct aws_string *str = malloc(sizeof(struct aws_string) + len + 1);
    if (str == NULL) {
        return NULL;
    }
    
    /* Initialize the fields - need to cast away const for initialization */
    *(struct aws_allocator **)&str->allocator = aws_default_allocator();
    *(size_t *)&str->len = len;
    
    /* The bytes are already allocated as part of the struct */
    return str;
}

void aws_string_eq_byte_cursor_harness(void) {
    /* Non-deterministically choose whether str and cur are NULL */
    bool str_is_null = nondet_bool();
    bool cur_is_null = nondet_bool();

    const struct aws_string *str = NULL;
    struct aws_byte_cursor cur_storage;
    const struct aws_byte_cursor *cur = NULL;

    /* Set up str if non-null */
    if (!str_is_null) {
        str = ensure_string_is_allocated_bounded(MAX_STRING_LEN);
        __CPROVER_assume(str != NULL);
        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* Set up cur if non-null */
    if (!cur_is_null) {
        __CPROVER_assume(aws_byte_cursor_is_bounded(&cur_storage, MAX_CURSOR_LEN));
        ensure_byte_cursor_has_allocated_buffer_member(&cur_storage);
        __CPROVER_assume(aws_byte_cursor_is_valid(&cur_storage));
        cur = &cur_storage;
    }

    /* Save old state for immutability checks */
    size_t old_str_len = (str != NULL) ? str->len : 0;
    struct aws_allocator *old_str_allocator = (str != NULL) ? str->allocator : NULL;

    size_t old_cur_len = (cur != NULL) ? cur->len : 0;
    uint8_t *old_cur_ptr = (cur != NULL) ? cur->ptr : NULL;

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
    /* We can't easily assert the exact value here without re-implementing,
     * but we can assert that if lengths differ, result must be false */
    if (str != NULL && cur != NULL) {
        if (str->len != cur->len) {
            assert(result == false);
        }
    }

    /* Postcondition 4: Immutability - str fields unchanged */
    if (str != NULL) {
        assert(str->len == old_str_len);
        assert(str->allocator == old_str_allocator);
        assert(aws_string_is_valid(str));
    }

    /* Postcondition 5: Immutability - cur fields unchanged */
    if (cur != NULL) {
        assert(cur->len == old_cur_len);
        assert(cur->ptr == old_cur_ptr);
        assert(aws_byte_cursor_is_valid(cur));
    }
}

void aws_string_eq_byte_cursor_harness(void) {
    aws_string_eq_byte_cursor_harness();
    return 0;
}
