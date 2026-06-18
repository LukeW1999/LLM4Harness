#include <proof_helpers/make_common_data_structures.h>
#include <string.h>

void aws_string_eq_byte_cursor_harness(void) {
    struct aws_string *str;
    struct aws_byte_cursor cursor;
    struct aws_allocator *allocator = aws_default_allocator();

    /* nondet decide whether str is NULL */
    if (__CPROVER_nondet_bool()) {
        str = NULL;
    } else {
        size_t len = __CPROVER_nondet_uint();
        __CPROVER_assume(len <= 256);
        uint8_t data[256];
        str = aws_string_new_from_array(allocator, data, len);
        __CPROVER_assume(str != NULL);
    }

    /* nondet decide whether cursor.ptr is NULL */
    if (__CPROVER_nondet_bool()) {
        cursor.ptr = NULL;
        cursor.len = 0;
    } else {
        size_t len = __CPROVER_nondet_uint();
        __CPROVER_assume(len <= 256);
        static uint8_t cur_data[256];
        cursor.ptr = cur_data;
        cursor.len = len;
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

    /* Postcondition: return‑value correctness */
    if (result) {
        __CPROVER_assert(
            (str != NULL && cursor.ptr != NULL &&
             str->len == cursor.len &&
             memcmp(str->bytes, cursor.ptr, str->len) == 0) ||
            ((str == NULL || str->len == 0) &&
             (cursor.ptr == NULL || cursor.len == 0)),
            "aws_string_eq_byte_cursor returned true only when strings are equal");
    } else {
        __CPROVER_assert(
            !((str != NULL && cursor.ptr != NULL &&
               str->len == cursor.len &&
               memcmp(str->bytes, cursor.ptr, str->len) == 0) ||
              ((str == NULL || str->len == 0) &&
               (cursor.ptr == NULL || cursor.len == 0))),
            "aws_string_eq_byte_cursor returned false only when strings differ");
    }

    /* Postcondition: length / pointer invariants */
    if (str != NULL) {
        __CPROVER_assert(str->len == old_str_len, "aws_string_eq_byte_cursor must not modify str->len");
    }
    __CPROVER_assert(cursor.len == old_cur_len, "aws_string_eq_byte_cursor must not modify cursor.len");
    __CPROVER_assert(cursor.ptr == old_cur_ptr, "aws_string_eq_byte_cursor must not modify cursor.ptr");

    /* Postcondition: memory frame conditions */
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

    if (str != NULL) {
        aws_string_destroy(str);
    }
}
