#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

#ifndef MAX_STRING_SIZE
#define MAX_STRING_SIZE 100
#endif

void aws_string_eq_byte_cursor_harness() {
    struct aws_allocator *allocator = aws_default_allocator();

    /* nondeterministic choices for pointer nullness */
    bool str_is_null;
    bool cur_is_null;

    /* create string using proof helper */
    struct aws_string *str = NULL;
    if (!str_is_null) {
        str = ensure_aws_string_is_allocated(allocator);
        __CPROVER_assume(str != NULL);
        __CPROVER_assume(str->len <= MAX_STRING_SIZE);
        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* create cursor */
    struct aws_byte_cursor cur;
    const struct aws_byte_cursor *cur_ptr = NULL;
    if (!cur_is_null) {
        cur.len = nondet_size_t();
        __CPROVER_assume(cur.len <= MAX_STRING_SIZE);
        if (cur.len > 0) {
            cur.ptr = malloc(cur.len);
            __CPROVER_assume(AWS_MEM_IS_READABLE(cur.ptr, cur.len));
        } else {
            cur.ptr = NULL;
        }
        cur_ptr = &cur;
        __CPROVER_assume(aws_byte_cursor_is_valid(cur_ptr));
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
        assert(aws_string_is_valid(str));
    }
    if (cur_ptr != NULL) {
        assert(aws_byte_cursor_is_valid(cur_ptr));
    }
}
