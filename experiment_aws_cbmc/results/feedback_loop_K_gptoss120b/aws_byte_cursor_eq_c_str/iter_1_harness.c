/*  
 * Contract for aws_byte_cursor_eq_c_str  
 * Preconditions:  
 *   - &cursor != NULL  
 *   - cursor->len is any size_t value (including 0)  
 *   - if cursor->len > 0 then cursor->ptr points to a valid memory region of at least cursor->len bytes  
 *   - c_str != NULL and points to a null‑terminated string (may be empty)  
 *   - The memory regions pointed to by cursor->ptr and c_str are not modified by the function  
 * Postconditions (validity):  
 *   - Return value equals the result of aws_array_eq_c_str(cursor->ptr, cursor->len, c_str)  
 * Postconditions (frame):  
 *   - cursor->ptr, cursor->len, and the contents of the memory they reference remain unchanged  
 *   - The contents of the null‑terminated string c_str remain unchanged  
 */

#include <aws/common/byte_buf.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_CURSOR_LEN 256
#define MAX_CSTR_LEN   256

void aws_byte_cursor_eq_c_str_harness(void) {
    /* Allocate and initialize aws_byte_cursor */
    struct aws_byte_cursor cursor;
    size_t cursor_len = __CPROVER_nondet_size_t();
    __CPROVER_assume(cursor_len <= MAX_CURSOR_LEN);

    uint8_t *cursor_buf = NULL;
    if (cursor_len > 0) {
        cursor_buf = malloc(cursor_len);
        __CPROVER_assume(cursor_buf != NULL);
        for (size_t i = 0; i < cursor_len; ++i) {
            cursor_buf[i] = __CPROVER_nondet_uint8_t();
        }
    }
    cursor.ptr = cursor_buf;
    cursor.len = cursor_len;

    /* Allocate and initialize null‑terminated C string */
    size_t cstr_len = __CPROVER_nondet_size_t();
    __CPROVER_assume(cstr_len <= MAX_CSTR_LEN);

    char *c_str = malloc(cstr_len + 1);
    __CPROVER_assume(c_str != NULL);
    for (size_t i = 0; i < cstr_len; ++i) {
        char ch = __CPROVER_nondet_char();
        __CPROVER_assume(ch != '\0');          /* ensure interior chars are non‑null */
        c_str[i] = ch;
    }
    c_str[cstr_len] = '\0';

    /* Snapshot state before the call */
    uint8_t *saved_cursor_buf = NULL;
    if (cursor_len > 0) {
        saved_cursor_buf = malloc(cursor_len);
        __CPROVER_assume(saved_cursor_buf != NULL);
        for (size_t i = 0; i < cursor_len; ++i) {
            saved_cursor_buf[i] = cursor_buf[i];
        }
    }
    size_t saved_cursor_len = cursor_len;
    struct aws_byte_cursor saved_cursor = cursor;

    char *saved_c_str = malloc(cstr_len + 1);
    __CPROVER_assume(saved_c_str != NULL);
    for (size_t i = 0; i <= cstr_len; ++i) {
        saved_c_str[i] = c_str[i];
    }

    /* Call the function under verification */
    bool result = aws_byte_cursor_eq_c_str(&cursor, c_str);

    /* Verify postconditions */
    /* Frame: cursor structure unchanged */
    __CPROVER_assert(cursor.ptr == saved_cursor.ptr, "cursor.ptr unchanged");
    __CPROVER_assert(cursor.len == saved_cursor.len, "cursor.len unchanged");

    /* Frame: cursor buffer contents unchanged */
    if (cursor_len > 0) {
        for (size_t i = 0; i < cursor_len; ++i) {
            __CPROVER_assert(cursor_buf[i] == saved_cursor_buf[i],
                             "cursor buffer byte unchanged");
        }
    }

    /* Frame: c_str contents unchanged */
    for (size_t i = 0; i <= cstr_len; ++i) {
        __CPROVER_assert(c_str[i] == saved_c_str[i],
                         "c_str byte unchanged");
    }

    /* Validity: result matches specification */
    bool expected = aws_array_eq_c_str(cursor.ptr, cursor.len, c_str);
    __CPROVER_assert(result == expected,
                     "return value matches aws_array_eq_c_str");

    return 0;
}
