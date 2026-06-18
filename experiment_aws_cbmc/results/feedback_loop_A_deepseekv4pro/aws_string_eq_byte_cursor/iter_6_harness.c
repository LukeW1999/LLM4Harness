#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_string_eq_byte_cursor_harness() {
    /* 1. Declare and bound data structures */
    struct aws_string *str = NULL;
    struct aws_byte_cursor cur;
    struct aws_byte_cursor *cur_ptr = NULL;

    /* Non-deterministically decide if str is NULL or allocated */
    bool str_is_null = nondet_bool();
    if (!str_is_null) {
        str = ensure_aws_string_is_allocated(MAX_BUFFER_SIZE);
        __CPROVER_assume(str != NULL);
        __CPROVER_assume(aws_string_is_valid(str));
        /* Ensure the actual bytes region is readable for its reported length */
        __CPROVER_assume(__CPROVER_r_ok(str->bytes, str->len));
    }

    /* Non-deterministically decide if cur is NULL or valid */
    bool cur_is_null = nondet_bool();
    if (!cur_is_null) {
        size_t cur_len = nondet_size_t();
        __CPROVER_assume(cur_len <= MAX_BUFFER_SIZE);
        uint8_t *buffer = malloc(cur_len);
        __CPROVER_assume(buffer != NULL);
        cur.ptr = buffer;
        cur.len = cur_len;
        __CPROVER_assume(aws_byte_cursor_is_valid(&cur));
        /* Ensure the cursor's data buffer is readable for its length */
        __CPROVER_assume(__CPROVER_r_ok(cur.ptr, cur.len));
        cur_ptr = &cur;
    }

    /* 2. Save old state BEFORE calling */
    size_t old_str_len = (str != NULL) ? str->len : 0;
    size_t old_cur_len = (cur_ptr != NULL) ? cur_ptr->len : 0;

    /* 3. Call function under test */
    bool result = aws_string_eq_byte_cursor(str, cur_ptr);

    /* 4. Assert postconditions */

    /* 5. Assert fields that must NOT change regardless of result */
    if (str != NULL) {
        assert(str->len == old_str_len);
    }
    if (cur_ptr != NULL) {
        assert(cur_ptr->len == old_cur_len);
    }

    /* 6. Assert validity invariants always hold */
    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }
    if (cur_ptr != NULL) {
        assert(aws_byte_cursor_is_valid(cur_ptr));
    }

    /* 7. Assert the return value matches the specification */
    if (str == NULL && cur_ptr == NULL) {
        assert(result == true);
    } else if (str == NULL || cur_ptr == NULL) {
        assert(result == false);
    } else {
        /* Both non-NULL: result should be true iff bytes match */
        if (str->len == cur_ptr->len) {
            /* If lengths match, check bytes */
            bool bytes_match = true;
            for (size_t i = 0; i < str->len; i++) {
                if (str->bytes[i] != cur_ptr->ptr[i]) {
                    bytes_match = false;
                    break;
                }
            }
            assert(result == bytes_match);
        } else {
            assert(result == false);
        }
    }

    /* 8. Free allocated memory */
    if (str != NULL) {
        free(str);
    }
    if (cur_ptr != NULL) {
        free(cur.ptr);
    }
}
