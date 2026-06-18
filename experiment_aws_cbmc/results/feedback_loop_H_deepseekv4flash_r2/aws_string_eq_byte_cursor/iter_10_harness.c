#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>

#ifndef MAX_STRING_SIZE
#define MAX_STRING_SIZE 100
#endif

void aws_string_eq_byte_cursor_harness() {
    struct aws_allocator *allocator = aws_default_allocator();

    /* nondeterministic choices for pointer nullness */
    bool str_is_null;
    bool cur_is_null;
    __CPROVER_assume(str_is_null == 0 || str_is_null == 1);
    __CPROVER_assume(cur_is_null == 0 || cur_is_null == 1);

    /* create string using proof helper */
    struct aws_string *str = NULL;
    if (!str_is_null) {
        str = ensure_aws_string_is_allocated(allocator);
        __CPROVER_assume(str != NULL);
        __CPROVER_assume(str->len <= MAX_STRING_SIZE);
    }

    /* create cursor */
    struct aws_byte_cursor cur;
    const struct aws_byte_cursor *cur_ptr = NULL;
    if (!cur_is_null) {
        cur = nondet_aws_byte_cursor();
        __CPROVER_assume(cur.len <= MAX_STRING_SIZE);
        if (cur.len > 0) {
            __CPROVER_assume(cur.ptr != NULL && AWS_MEM_IS_READABLE(cur.ptr, cur.len));
        }
        cur_ptr = &cur;
    }

    /* save old state for immutability */
    struct store_byte_from_buffer saved_str;
    struct store_byte_from_buffer saved_cur;
    if (str != NULL) {
        save_byte_from_array(str->bytes, str->len, &saved_str);
    }
    if (cur_ptr != NULL && cur.len > 0) {
        save_byte_from_array(cur.ptr, cur.len, &saved_cur);
    }

    /* call function */
    bool result = aws_string_eq_byte_cursor(str, cur_ptr);

    /* postconditions */
    if (str_is_null && cur_is_null) {
        assert(result == true);
    } else if (str_is_null || cur_is_null) {
        assert(result == false);
    } else {
        bool eq = (str->len == cur.len);
        if (eq) {
            size_t i;
            bool all_equal = true;
            for (i = 0; i < str->len; i++) {
                if (str->bytes[i] != cur.ptr[i]) {
                    all_equal = false;
                    break;
                }
            }
            assert(result == all_equal);
        } else {
            assert(result == false);
        }
    }

    /* immutability */
    if (str != NULL) {
        assert_byte_from_buffer_matches(str->bytes, &saved_str);
    }
    if (cur_ptr != NULL && cur.len > 0) {
        assert_byte_from_buffer_matches(cur.ptr, &saved_cur);
    }

    /* validity */
    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }
    if (cur_ptr != NULL) {
        assert(aws_byte_cursor_is_valid(cur_ptr));
    }
}
