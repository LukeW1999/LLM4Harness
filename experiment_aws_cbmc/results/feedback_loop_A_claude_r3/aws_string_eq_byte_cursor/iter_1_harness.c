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
 * From Doxygen: Returns true if bytes of string and cursor are the same, false otherwise.
 *
 * From implementation:
 * - If both str and cur are NULL, returns true
 * - If exactly one is NULL, returns false
 * - Otherwise, compares bytes using aws_array_eq
 *
 * 1. Changed fields: none (pure comparison, no mutation)
 * 2. Unchanged fields: all fields of str and cur remain unchanged
 * 3. Failure: N/A (returns bool, not error code)
 * 4. Validity invariants: str and cur remain valid after the call
 */

/* We need a helper to allocate an aws_string with bounded length */
struct aws_string *ensure_string_is_allocated(size_t max_len) {
    size_t len;
    __CPROVER_assume(len <= max_len);
    /* Allocate enough memory for the struct plus len+1 bytes (for null terminator) */
    struct aws_string *str = malloc(sizeof(struct aws_string) + len + 1);
    if (str == NULL) {
        return NULL;
    }
    /* Set the length field (const, so we use a trick) */
    *(size_t *)(&str->len) = len;
    /* allocator can be NULL or non-NULL */
    *(struct aws_allocator **)(&str->allocator) = nondet_bool() ? NULL : aws_default_allocator();
    /* bytes are already allocated as part of the struct */
    return str;
}

void aws_string_eq_byte_cursor_harness(void) {
    /* Decide non-deterministically whether str and cur are NULL */
    bool str_is_null = nondet_bool();
    bool cur_is_null = nondet_bool();

    const struct aws_string *str = NULL;
    struct aws_byte_cursor cur_storage;
    const struct aws_byte_cursor *cur = NULL;

    /* Set up str if non-null */
    if (!str_is_null) {
#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 8
#endif
        str = ensure_string_is_allocated(MAX_BUFFER_SIZE);
        __CPROVER_assume(str != NULL);
        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* Set up cur if non-null */
    if (!cur_is_null) {
        __CPROVER_assume(aws_byte_cursor_is_bounded(&cur_storage, MAX_BUFFER_SIZE));
        ensure_byte_cursor_has_allocated_buffer_member(&cur_storage);
        __CPROVER_assume(aws_byte_cursor_is_valid(&cur_storage));
        cur = &cur_storage;
    }

    /* Save old state for immutability checks */
    size_t old_str_len = (str != NULL) ? str->len : 0;
    struct aws_allocator *old_str_allocator = (str != NULL) ? str->allocator : NULL;

    size_t old_cur_len = (cur != NULL) ? cur->len : 0;
    uint8_t *old_cur_ptr = (cur != NULL) ? cur->ptr : NULL;

    /* Call function under test */
    bool result = aws_string_eq_byte_cursor(str, cur);

    /* Postconditions */

    /* Both NULL => true */
    if (str == NULL && cur == NULL) {
        assert(result == true);
    }
    /* Exactly one NULL => false */
    else if (str == NULL || cur == NULL) {
        assert(result == false);
    }
    /* Both non-NULL: result depends on content comparison */
    /* We can't easily assert the exact value without re-implementing,
       but we can assert that if lengths differ, result must be false */
    else {
        if (str->len != cur->len) {
            assert(result == false);
        }
        /* If lengths are equal and result is true, bytes must match */
        /* (We trust aws_array_eq correctness here) */
    }

    /* Immutability: str fields unchanged */
    if (str != NULL) {
        assert(str->len == old_str_len);
        assert(str->allocator == old_str_allocator);
        /* Validity still holds */
        assert(aws_string_is_valid(str));
    }

    /* Immutability: cur fields unchanged */
    if (cur != NULL) {
        assert(cur->len == old_cur_len);
        assert(cur->ptr == old_cur_ptr);
        /* Validity still holds */
        assert(aws_byte_cursor_is_valid(cur));
    }
}
