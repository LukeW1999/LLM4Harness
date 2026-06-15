#include <aws/common/byte_buf.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "proof_helpers/make_common_data_structures.h"

#define MAX_BUFFER_SIZE 1024
#define MAX_CSTR_SIZE   1024

void aws_byte_cursor_eq_c_str_harness(void) {
    /*--- Set up aws_byte_cursor ------------------------------------------------*/
    struct aws_byte_cursor cur;
    cur.len = nondet_size_t();
    __CPROVER_assume(cur.len <= MAX_BUFFER_SIZE);

    cur.ptr = (uint8_t *)malloc(cur.len);
    __CPROVER_assume(cur.ptr != NULL);               /* allocation must succeed */
    /* contents of cur.ptr are nondet, no need to initialise */

    /*--- Set up null‑terminated C string ---------------------------------------*/
    size_t c_str_len = nondet_size_t();
    __CPROVER_assume(c_str_len <= MAX_CSTR_SIZE);

    char *c_str = (char *)malloc(c_str_len + 1);
    __CPROVER_assume(c_str != NULL);
    /* make it a proper C string */
    c_str[c_str_len] = '\0';
    /* the characters before the terminator are nondet */

    /*--- Apply the given pre‑conditions ---------------------------------------*/
    __CPROVER_assume(c_str != NULL);
    __CPROVER_assume(aws_byte_cursor_is_bounded(&cur, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_cursor_is_valid(&cur));

    /*--- Snapshot memory that must stay unchanged ------------------------------*/
    uint8_t *old_buf = NULL;
    if (cur.len > 0) {
        old_buf = (uint8_t *)malloc(cur.len);
        __CPROVER_assume(old_buf != NULL);
        memcpy(old_buf, cur.ptr, cur.len);
    }

    char *old_cstr = (char *)malloc(c_str_len + 1);
    __CPROVER_assume(old_cstr != NULL);
    memcpy(old_cstr, c_str, c_str_len + 1);

    /*--- Call the function under verification ----------------------------------*/
    bool result = aws_byte_cursor_eq_c_str(&cur, c_str);

    /*--- Post‑conditions -------------------------------------------------------*/

    /* 1. Validity predicates must still hold */
    assert(aws_byte_cursor_is_valid(&cur));

    /* 2. No memory modification (frame condition) */
    if (cur.len > 0) {
        assert(memcmp(cur.ptr, old_buf, cur.len) == 0);
    }
    assert(memcmp(c_str, old_cstr, c_str_len + 1) == 0);

    /* 3. Return‑value correctness */
    /* compute length of c_str (up to the terminating NUL) */
    size_t c_len = 0;
    while (c_str[c_len] != '\0') {
        c_len++;
    }

    bool expected = (c_len == cur.len);
    if (expected) {
        for (size_t i = 0; i < cur.len; ++i) {
            if (cur.ptr[i] != (uint8_t)c_str[i]) {
                expected = false;
                break;
            }
        }
    }
    assert(result == expected);

    return 0;
}
