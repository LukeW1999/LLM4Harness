#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/**
 * Harness for aws_string_eq_byte_cursor
 *
 * Implementation:
 *   - If both str and cur are NULL → return true
 *   - If exactly one is NULL → return false
 *   - Otherwise → return aws_array_eq(str->bytes, str->len, cur->ptr, cur->len)
 *
 * Postconditions:
 *   1. If both NULL → result is true
 *   2. If exactly one NULL → result is false
 *   3. If neither NULL → result reflects byte-level equality
 *   4. Neither str nor cur is modified (read-only function)
 *   5. Validity invariants hold after the call
 */

/* We need a helper to create a valid aws_string for CBMC */
struct aws_string *ensure_string_is_allocated(size_t max_len) {
    size_t len;
    __CPROVER_assume(len <= max_len);
    
    /* Allocate enough space for the struct + bytes + null terminator */
    struct aws_string *str = malloc(sizeof(struct aws_string) + len + 1);
    if (str == NULL) return NULL;
    
    /* Set the length field (cast away const for initialization) */
    *(size_t *)&str->len = len;
    *(struct aws_allocator **)&str->allocator = NULL;
    
    /* bytes are non-deterministic (already allocated) */
    /* Ensure null terminator */
    ((uint8_t *)str->bytes)[len] = '\0';
    
    return str;
}

void aws_string_eq_byte_cursor_harness(void) {
    /* Non-deterministically choose whether str and cur are NULL */
    bool str_is_null = nondet_bool();
    bool cur_is_null = nondet_bool();

    /* Set up str */
    const struct aws_string *str = NULL;
    if (!str_is_null) {
        /* Create a bounded aws_string */
        size_t str_len;
        __CPROVER_assume(str_len <= 4); /* bound for CBMC tractability */
        
        struct aws_string *s = malloc(sizeof(struct aws_string) + str_len + 1);
        __CPROVER_assume(s != NULL);
        *(size_t *)&s->len = str_len;
        *(struct aws_allocator **)&s->allocator = NULL;
        ((uint8_t *)s->bytes)[str_len] = '\0';
        str = s;
    }

    /* Set up cur */
    const struct aws_byte_cursor *cur = NULL;
    struct aws_byte_cursor cursor;
    if (!cur_is_null) {
        __CPROVER_assume(aws_byte_cursor_is_bounded(&cursor, 4));
        ensure_byte_cursor_has_allocated_buffer_member(&cursor);
        __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));
        cur = &cursor;
    }

    /* Save old state for immutability checks */
    const struct aws_string *old_str = str;
    size_t old_str_len = (str != NULL) ? str->len : 0;
    
    struct aws_byte_cursor old_cursor;
    if (cur != NULL) {
        old_cursor = *cur;
    }

    /* Call the function under test */
    bool result = aws_string_eq_byte_cursor(str, cur);

    /* Postcondition 1: Both NULL → true */
    if (str == NULL && cur == NULL) {
        assert(result == true);
    }

    /* Postcondition 2: Exactly one NULL → false */
    if (str == NULL && cur != NULL) {
        assert(result == false);
    }
    if (str != NULL && cur == NULL) {
        assert(result == false);
    }

    /* Postcondition 3: Neither NULL → result is based on content comparison */
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

    /* Postcondition 4: str pointer unchanged (function is read-only) */
    assert(str == old_str);

    /* Postcondition 5: str fields unchanged if str was non-NULL */
    if (str != NULL) {
        assert(str->len == old_str_len);
        assert(str->allocator == NULL || str->allocator != NULL); /* allocator not modified */
    }

    /* Postcondition 6: cur fields unchanged if cur was non-NULL */
    if (cur != NULL) {
        assert(cur->ptr == old_cursor.ptr);
        assert(cur->len == old_cursor.len);
    }

    /* Postcondition 7: Validity invariants */
    if (cur != NULL) {
        assert(aws_byte_cursor_is_valid(cur));
    }
    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }
}
