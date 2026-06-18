#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

/**
 * Harness for aws_string_eq_byte_cursor
 *
 * Function returns true if bytes of string and cursor are the same, false otherwise.
 * Special cases:
 *   - Both NULL → true
 *   - One NULL → false
 *   - Otherwise → compare bytes
 *
 * Nothing is modified; both inputs are read-only.
 */
void aws_string_eq_byte_cursor_harness(void) {
    /* 1. Set up aws_string */
    /* We need to allocate an aws_string with a bounded length */
    struct aws_string *str = NULL;
    bool str_is_null = nondet_bool();

    if (!str_is_null) {
        /* Allocate a string with bounded length */
        size_t str_len = nondet_size_t();
        __CPROVER_assume(str_len <= MAX_BUFFER_SIZE);

        /* Allocate memory for the string header + bytes + null terminator */
        str = malloc(sizeof(struct aws_string) + str_len + 1);
        __CPROVER_assume(str != NULL);

        /* Initialize the string fields (cast away const for initialization) */
        *(struct aws_allocator **)&str->allocator = aws_default_allocator();
        *(size_t *)&str->len = str_len;
        /* bytes are nondet (already nondet from malloc) */
        /* ensure null terminator */
        ((uint8_t *)str->bytes)[str_len] = 0;

        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* 2. Set up aws_byte_cursor */
    struct aws_byte_cursor *cur = NULL;
    bool cur_is_null = nondet_bool();

    struct aws_byte_cursor cursor_val;
    if (!cur_is_null) {
        __CPROVER_assume(aws_byte_cursor_is_bounded(&cursor_val, MAX_BUFFER_SIZE));
        ensure_byte_cursor_has_allocated_buffer_member(&cursor_val);
        __CPROVER_assume(aws_byte_cursor_is_valid(&cursor_val));
        cur = &cursor_val;
    }

    /* 3. Save old state (inputs are not modified, but let's record for postconditions) */
    size_t old_str_len = (str != NULL) ? str->len : 0;
    size_t old_cur_len = (cur != NULL) ? cur->len : 0;
    uint8_t *old_cur_ptr = (cur != NULL) ? cur->ptr : NULL;

    /* 4. Call function under test */
    bool result = aws_string_eq_byte_cursor(str, cur);

    /* 5. Assert postconditions */

    /* Both NULL → must return true */
    if (str == NULL && cur == NULL) {
        assert(result == true);
    }
    /* One NULL → must return false */
    else if (str == NULL || cur == NULL) {
        assert(result == false);
    }
    /* Both non-NULL → result depends on content comparison */
    else {
        /* If lengths differ, must be false */
        if (str->len != cur->len) {
            assert(result == false);
        }
        /* If lengths are zero, must be true */
        if (str->len == 0 && cur->len == 0) {
            assert(result == true);
        }
        /* General case: result is valid bool */
        assert(result == true || result == false);
    }

    /* 6. Assert inputs are unchanged (function is read-only) */
    if (str != NULL) {
        assert(str->len == old_str_len);
        assert(aws_string_is_valid(str));
    }
    if (cur != NULL) {
        assert(cur->len == old_cur_len);
        assert(cur->ptr == old_cur_ptr);
        assert(aws_byte_cursor_is_valid(cur));
    }
}
