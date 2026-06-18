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
 * Function behavior:
 * - If both str and cur are NULL, returns true
 * - If exactly one is NULL, returns false
 * - Otherwise, returns true iff str->bytes[0..len-1] == cur->ptr[0..len-1] and str->len == cur->len
 *
 * The function does not modify any inputs.
 */
void aws_string_eq_byte_cursor_harness(void) {
    /* 1. Set up aws_string */
    struct aws_string *str = NULL;
    bool str_is_null = nondet_bool();

    if (!str_is_null) {
        /* Allocate a string with bounded length */
        size_t str_len = nondet_size_t();
        __CPROVER_assume(str_len <= MAX_BUFFER_SIZE);

        /* Allocate memory for the aws_string header + bytes + null terminator */
        str = malloc(sizeof(struct aws_string) + str_len + 1);
        __CPROVER_assume(str != NULL);

        /* Initialize the string fields (cast away const for initialization) */
        *(struct aws_allocator **)&str->allocator = aws_default_allocator();
        *(size_t *)&str->len = str_len;
        /* bytes are nondet (already nondet from malloc) */
        /* ensure null terminator */
        ((uint8_t *)str->bytes)[str_len] = '\0';

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

    /* 3. Save old state (inputs should not be modified) */
    size_t old_str_len = (str != NULL) ? str->len : 0;
    size_t old_cur_len = (cur != NULL) ? cur->len : 0;
    uint8_t *old_cur_ptr = (cur != NULL) ? cur->ptr : NULL;

    /* 4. Call function under test */
    bool result = aws_string_eq_byte_cursor(str, cur);

    /* 5. Assert postconditions */

    /* Case: both NULL → true */
    if (str == NULL && cur == NULL) {
        assert(result == true);
    }
    /* Case: exactly one NULL → false */
    else if (str == NULL || cur == NULL) {
        assert(result == false);
    }
    /* Case: neither NULL → result depends on content */
    else {
        /* If lengths differ, must be false */
        if (str->len != cur->len) {
            assert(result == false);
        }
        /* result is either true or false — both are valid depending on bytes */
        /* We can assert that if result is true, lengths must match */
        if (result == true) {
            assert(str->len == cur->len);
        }
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
