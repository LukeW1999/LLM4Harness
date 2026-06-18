#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

void aws_string_eq_byte_cursor_harness(void) {
    struct aws_string *str;
    struct aws_byte_cursor cursor;

    /* nondet decide whether str is NULL */
    if (__CPROVER_nondet_bool()) {
        str = NULL;
    } else {
        size_t len = __CPROVER_nondet_uint();
        __CPROVER_assume(len <= 256);
        uint8_t *mem = malloc(sizeof(struct aws_string) + len);
        __CPROVER_assume(mem != NULL);
        str = (struct aws_string *)mem;
        str->len = len;
        /* bytes are nondet, no need to initialise */
    }

    /* nondet decide whether cursor.ptr is NULL */
    if (__CPROVER_nondet_bool()) {
        cursor.ptr = NULL;
        cursor.len = 0;
    } else {
        size_t len = __CPROVER_nondet_uint();
        __CPROVER_assume(len <= 256);
        cursor.ptr = malloc(len);
        __CPROVER_assume(cursor.ptr != NULL);
        cursor.len = len;
        /* bytes are nondet, no need to initialise */
    }

    /* Save copies for frame conditions */
    size_t old_str_len = 0;
    uint8_t old_str_bytes[256] = {0};
    if (str != NULL) {
        old_str_len = str->len;
        size_t copy_len = str->len < sizeof(old_str_bytes) ? str->len : sizeof(old_str_bytes);
        memcpy(old_str_bytes, str->bytes, copy_len);
    }

    const uint8_t *old_cur_ptr = cursor.ptr;
    size_t old_cur_len = cursor.len;
    uint8_t old_cur_bytes[256] = {0};
    if (cursor.ptr != NULL) {
        size_t copy_len = cursor.len < sizeof(old_cur_bytes) ? cursor.len : sizeof(old_cur_bytes);
        memcpy(old_cur_bytes, cursor.ptr, copy_len);
    }

    /* Call the function under verification */
    bool result = aws_string_eq_byte_cursor(str, &cursor);

    /* Postcondition 1: return‑value correctness */
    if (result) {
        __CPROVER_assert(
            (str == NULL && cursor.ptr == NULL) ||
            (str != NULL && cursor.ptr != NULL &&
             str->len == cursor.len &&
             memcmp(str->bytes, cursor.ptr, str->len) == 0),
            "aws_string_eq_byte_cursor returned true only when strings are equal");
    } else {
        __CPROVER_assert(
            !((str == NULL && cursor.ptr == NULL) ||
              (str != NULL && cursor.ptr != NULL &&
               str->len == cursor.len &&
               memcmp(str->bytes, cursor.ptr, str->len) == 0)),
            "aws_string_eq_byte_cursor returned false only when strings differ");
    }

    /* Postcondition 2: length / pointer invariants */
    if (str != NULL) {
        __CPROVER_assert(str->len == old_str_len, "aws_string_eq_byte_cursor must not modify str->len");
    }
    __CPROVER_assert(cursor.len == old_cur_len, "aws_string_eq_byte_cursor must not modify cursor.len");
    __CPROVER_assert(cursor.ptr == old_cur_ptr, "aws_string_eq_byte_cursor must not modify cursor.ptr");

    /* Postcondition 3: memory frame conditions */
    if (str != NULL) {
        size_t copy_len = str->len < sizeof(old_str_bytes) ? str->len : sizeof(old_str_bytes);
        __CPROVER_assert(memcmp(str->bytes, old_str_bytes, copy_len) == 0,
                         "aws_string_eq_byte_cursor must not modify string bytes");
    }
    if (cursor.ptr != NULL) {
        size_t copy_len = cursor.len < sizeof(old_cur_bytes) ? cursor.len : sizeof(old_cur_bytes);
        __CPROVER_assert(memcmp(cursor.ptr, old_cur_bytes, copy_len) == 0,
                         "aws_string_eq_byte_cursor must not modify cursor bytes");
    }
}
