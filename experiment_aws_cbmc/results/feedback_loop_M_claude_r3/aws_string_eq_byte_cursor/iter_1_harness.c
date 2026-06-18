#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_string_eq_byte_cursor_harness() {
    /* Case 1: both NULL */
    /* Case 2: str NULL, cur non-NULL */
    /* Case 3: str non-NULL, cur NULL */
    /* Case 4: both non-NULL */

    /* Non-deterministically choose which case to test */
    bool str_is_null = nondet_bool();
    bool cur_is_null = nondet_bool();

    struct aws_string *str = NULL;
    struct aws_byte_cursor *cur = NULL;

    /* Build a valid aws_string if needed */
    struct aws_string *str_ptr = NULL;
    if (!str_is_null) {
        /* Allocate an aws_string with bounded length */
        size_t str_len = nondet_size_t();
        __CPROVER_assume(str_len <= MAX_BUFFER_SIZE);

        /* Allocate memory for the aws_string header + bytes + null terminator */
        str_ptr = malloc(sizeof(struct aws_string) + str_len + 1);
        __CPROVER_assume(str_ptr != NULL);

        /* Initialize the fields (cast away const for initialization) */
        *(struct aws_allocator **)&str_ptr->allocator = aws_default_allocator();
        *(size_t *)&str_ptr->len = str_len;
        /* bytes are nondet (already nondet from malloc) */
        /* ensure null terminator */
        ((uint8_t *)str_ptr->bytes)[str_len] = '\0';

        __CPROVER_assume(aws_string_is_valid(str_ptr));
        str = str_ptr;
    }

    /* Build a valid aws_byte_cursor if needed */
    struct aws_byte_cursor cursor;
    if (!cur_is_null) {
        __CPROVER_assume(aws_byte_cursor_is_bounded(&cursor, MAX_BUFFER_SIZE));
        ensure_byte_cursor_has_allocated_buffer_member(&cursor);
        __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));
        cur = &cursor;
    }

    /* Call the function under test */
    bool result = aws_string_eq_byte_cursor(str, cur);

    /* Postconditions based on Doxygen and implementation */

    /* If both NULL, must return true */
    if (str == NULL && cur == NULL) {
        assert(result == true);
    }

    /* If exactly one is NULL, must return false */
    if (str == NULL && cur != NULL) {
        assert(result == false);
    }
    if (str != NULL && cur == NULL) {
        assert(result == false);
    }

    /* If both non-NULL, result depends on content comparison */
    if (str != NULL && cur != NULL) {
        /* The function returns true iff bytes match */
        /* We can assert: if lengths differ, result must be false */
        if (str->len != cur->len) {
            assert(result == false);
        }
        /* If lengths are both 0, result must be true */
        if (str->len == 0 && cur->len == 0) {
            assert(result == true);
        }
    }

    /* Validity invariants: inputs must not be modified */
    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }
    if (cur != NULL) {
        assert(aws_byte_cursor_is_valid(cur));
    }
}
