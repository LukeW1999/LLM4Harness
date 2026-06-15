#include <aws/common/string.h>
#include <aws/common/byte_cursor.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_string_eq_byte_cursor_harness(void) {
    /* nondeterministic string pointer */
    struct aws_string *str = NULL;
    if (__CPROVER_nondet_bool()) {
        /* allocate a string with a nondeterministic length */
        size_t len = __CPROVER_nondet_size_t();
        __CPROVER_assume(len < 256);               /* keep allocation bounded */
        str = (struct aws_string *)__CPROVER_allocate(
            sizeof(struct aws_string) + len, 0);
        str->allocator = aws_default_allocator();
        str->len = len;
        for (size_t i = 0; i < len; ++i) {
            ((uint8_t *)str->bytes)[i] = __CPROVER_nondet_uint8_t();
        }
    }

    /* nondeterministic byte cursor */
    struct aws_byte_cursor cursor;
    cursor.ptr = NULL;
    cursor.len = 0;
    if (__CPROVER_nondet_bool()) {
        cursor.len = __CPROVER_nondet_size_t();
        __CPROVER_assume(cursor.len < 256);
        cursor.ptr = (uint8_t *)__CPROVER_allocate(cursor.len, 0);
        for (size_t i = 0; i < cursor.len; ++i) {
            ((uint8_t *)cursor.ptr)[i] = __CPROVER_nondet_uint8_t();
        }
    }

    /* structural validity assumptions */
    __CPROVER_assume(IMPLIES(str != NULL, aws_string_is_valid(str)));
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));

    /* save copies for frame condition checks */
    struct aws_string old_str_copy;
    uint8_t *old_str_bytes = NULL;
    if (str) {
        old_str_copy = *str;
        if (str->len > 0) {
            old_str_bytes = (uint8_t *)malloc(str->len);
            memcpy(old_str_bytes, str->bytes, str->len);
        }
    }
    struct aws_byte_cursor old_cur = cursor;
    uint8_t *old_cur_bytes = NULL;
    if (cursor.ptr) {
        old_cur_bytes = (uint8_t *)malloc(cursor.len);
        memcpy(old_cur_bytes, cursor.ptr, cursor.len);
    }

    /* call the function under verification */
    bool result = aws_string_eq_byte_cursor(str, &cursor);

    /* post‑condition: return‑value correctness */
    if (str == NULL && cursor.ptr == NULL) {
        __CPROVER_assert(result == true,
                         "aws_string_eq_byte_cursor returns true when both arguments are NULL");
    } else if (str == NULL || cursor.ptr == NULL) {
        __CPROVER_assert(result == false,
                         "aws_string_eq_byte_cursor returns false when exactly one argument is NULL");
    } else {
        bool expected = aws_array_eq(str->bytes, str->len, cursor.ptr, cursor.len);
        __CPROVER_assert(result == expected,
                         "aws_string_eq_byte_cursor returns the result of aws_array_eq for non‑NULL arguments");
    }

    /* frame condition: the string must not be modified */
    if (str) {
        __CPROVER_assert(str->len == old_str_copy.len,
                         "aws_string_eq_byte_cursor does not modify str->len");
        __CPROVER_assert(str->allocator == old_str_copy.allocator,
                         "aws_string_eq_byte_cursor does not modify str->allocator");
        if (str->len > 0) {
            __CPROVER_assert(
                memcmp(str->bytes, old_str_bytes, str->len) == 0,
                "aws_string_eq_byte_cursor does not modify str->bytes");
        }
    }

    /* frame condition: the cursor must not be modified */
    __CPROVER_assert(cursor.ptr == old_cur.ptr,
                     "aws_string_eq_byte_cursor does not modify cursor.ptr");
    __CPROVER_assert(cursor.len == old_cur.len,
                     "aws_string_eq_byte_cursor does not modify cursor.len");
    if (cursor.ptr && cursor.len > 0) {
        __CPROVER_assert(
            memcmp(cursor.ptr, old_cur_bytes, cursor.len) == 0,
            "aws_string_eq_byte_cursor does not modify cursor buffer");
    }

    return 0;
}
