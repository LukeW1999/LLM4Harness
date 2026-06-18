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
    }

    /* Non-deterministically decide if cur is NULL or valid */
    bool cur_is_null = nondet_bool();
    if (!cur_is_null) {
        /* Use a stack buffer for the cursor's data */
        uint8_t buffer[MAX_BUFFER_SIZE];
        cur.ptr = buffer;
        cur.len = nondet_size_t();
        __CPROVER_assume(cur.len <= MAX_BUFFER_SIZE);
        __CPROVER_assume(aws_byte_cursor_is_valid(&cur));
        cur_ptr = &cur;
    }

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    size_t old_str_len = (str != NULL) ? str->len : 0;
    uint8_t *old_str_bytes = (str != NULL) ? str->bytes : NULL;
    size_t old_cur_len = (cur_ptr != NULL) ? cur_ptr->len : 0;
    uint8_t *old_cur_ptr = (cur_ptr != NULL) ? cur_ptr->ptr : NULL;

    /* 3. Call function under test */
    bool result = aws_string_eq_byte_cursor(str, cur_ptr);

    /* 4. Assert postconditions */

    /* 5. Assert fields that must NOT change regardless of result */
    if (str != NULL) {
        assert(str->len == old_str_len);
        assert(str->bytes == old_str_bytes);
    }
    if (cur_ptr != NULL) {
        assert(cur_ptr->len == old_cur_len);
        assert(cur_ptr->ptr == old_cur_ptr);
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
    /* cur buffer is stack-allocated, no free needed */
}
