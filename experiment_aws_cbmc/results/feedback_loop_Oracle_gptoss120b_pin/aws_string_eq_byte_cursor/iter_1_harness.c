#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <aws/common/byte_cursor.h>
#include <aws/common/allocator.h>
#include <aws/common/common.h>
#include <aws/common/array.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_LEN 256

void aws_string_eq_byte_cursor_harness(void) {
    /* nondet allocation for aws_string */
    struct aws_string *str;
    size_t str_len = nondet_size_t();
    __CPROVER_assume(str_len <= MAX_LEN);
    bool str_is_null = nondet_bool();
    if (str_is_null) {
        str = NULL;
    } else {
        /* allocate space for struct + bytes (flexible array) */
        str = malloc(sizeof(struct aws_string) + str_len);
        __CPROVER_assume(str != NULL);
        str->allocator = NULL;               /* static string */
        str->len = str_len;
        for (size_t i = 0; i < str_len; ++i) {
            ((uint8_t *)str->bytes)[i] = nondet_uint8_t();
        }
        ((uint8_t *)str->bytes)[str_len] = 0; /* null‑terminator (not counted) */
    }

    /* nondet allocation for aws_byte_cursor */
    struct aws_byte_cursor cursor;
    size_t cur_len = nondet_size_t();
    __CPROVER_assume(cur_len <= MAX_LEN);
    bool cur_ptr_is_null = nondet_bool();
    if (cur_ptr_is_null) {
        cursor.ptr = NULL;
        cursor.len = 0;
    } else {
        cursor.ptr = malloc(cur_len);
        __CPROVER_assume(cursor.ptr != NULL);
        cursor.len = cur_len;
        for (size_t i = 0; i < cur_len; ++i) {
            ((uint8_t *)cursor.ptr)[i] = nondet_uint8_t();
        }
    }

    /* structural validity assumptions */
    __CPROVER_assume(IMPLIES(str != NULL, aws_string_is_valid(str)));
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));

    /* snapshot of pre‑state */
    size_t pre_str_len = 0;
    struct aws_allocator *pre_str_alloc = NULL;
    uint8_t *pre_str_bytes = NULL;
    if (str != NULL) {
        pre_str_len = str->len;
        pre_str_alloc = str->allocator;
        pre_str_bytes = malloc(pre_str_len);
        __CPROVER_assume(pre_str_bytes != NULL);
        for (size_t i = 0; i < pre_str_len; ++i) {
            pre_str_bytes[i] = ((uint8_t *)str->bytes)[i];
        }
    }

    uint8_t *pre_cur_bytes = NULL;
    size_t pre_cur_len = cursor.len;
    void *pre_cur_ptr = cursor.ptr;
    if (cursor.ptr != NULL) {
        pre_cur_bytes = malloc(pre_cur_len);
        __CPROVER_assume(pre_cur_bytes != NULL);
        for (size_t i = 0; i < pre_cur_len; ++i) {
            pre_cur_bytes[i] = ((uint8_t *)cursor.ptr)[i];
        }
    }

    /* call the function under verification */
    bool result = aws_string_eq_byte_cursor(str, &cursor);

    /* compute expected result according to specification */
    bool expected;
    if (str == NULL && cursor.ptr == NULL) {
        expected = true;
    } else if (str == NULL || cursor.ptr == NULL) {
        expected = false;
    } else {
        expected = aws_array_eq(str->bytes, str->len, cursor.ptr, cursor.len);
    }

    /* post‑condition: return value matches specification */
    assert(result == expected);

    /* frame conditions: str unchanged */
    if (str != NULL) {
        assert(str->len == pre_str_len);
        assert(str->allocator == pre_str_alloc);
        for (size_t i = 0; i < pre_str_len; ++i) {
            assert(((uint8_t *)str->bytes)[i] == pre_str_bytes[i]);
        }
    }

    /* frame conditions: cursor unchanged */
    assert(cursor.len == pre_cur_len);
    assert(cursor.ptr == pre_cur_ptr);
    if (cursor.ptr != NULL) {
        for (size_t i = 0; i < pre_cur_len; ++i) {
            assert(((uint8_t *)cursor.ptr)[i] == pre_cur_bytes[i]);
        }
    }

    return 0;
}
