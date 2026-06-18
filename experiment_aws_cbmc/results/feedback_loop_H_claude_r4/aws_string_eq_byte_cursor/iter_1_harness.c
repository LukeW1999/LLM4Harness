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
#    define MAX_STRING_LEN 10
#endif

#ifndef MAX_CURSOR_LEN
#    define MAX_CURSOR_LEN 10
#endif

void aws_string_eq_byte_cursor_harness(void) {
    /* Non-deterministically decide whether str and cur are NULL */
    bool str_is_null = nondet_bool();
    bool cur_is_null = nondet_bool();

    const struct aws_string *str = NULL;
    struct aws_byte_cursor cur_val;
    struct aws_byte_cursor *cur = NULL;

    /* Set up str if non-null */
    if (!str_is_null) {
        /* Allocate an aws_string non-deterministically */
        /* aws_string has a flexible array member; we allocate a struct with extra bytes */
        size_t str_len;
        __CPROVER_assume(str_len <= MAX_STRING_LEN);

        /* Allocate memory for the aws_string header + bytes + null terminator */
        struct aws_string *s = malloc(sizeof(struct aws_string) + str_len + 1);
        __CPROVER_assume(s != NULL);

        /* Initialize the fields (cast away const for initialization) */
        *(struct aws_allocator **)&s->allocator = aws_default_allocator();
        *(size_t *)&s->len = str_len;
        /* bytes are non-deterministic (already non-det from malloc) */
        /* ensure null terminator */
        ((uint8_t *)s->bytes)[str_len] = 0;

        __CPROVER_assume(aws_string_is_valid(s));
        str = s;
    }

    /* Set up cur if non-null */
    if (!cur_is_null) {
        __CPROVER_assume(aws_byte_cursor_is_bounded(&cur_val, MAX_CURSOR_LEN));
        ensure_byte_cursor_has_allocated_buffer_member(&cur_val);
        __CPROVER_assume(aws_byte_cursor_is_valid(&cur_val));
        cur = &cur_val;
    }

    /* Save old state for immutability checks */
    const struct aws_string *old_str = str;
    struct aws_byte_cursor old_cur_val;
    if (cur != NULL) {
        old_cur_val = *cur;
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

    /* Immutability: str pointer unchanged */
    assert(str == old_str);

    /* Immutability: cur fields unchanged */
    if (cur != NULL) {
        assert(cur->ptr == old_cur_val.ptr);
        assert(cur->len == old_cur_val.len);
    }

    /* Validity invariants still hold */
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
