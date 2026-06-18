/*  
Preconditions:  
- __CPROVER_assume(cursor != NULL);  
- __CPROVER_assume(c_str != NULL);  
- __CPROVER_assume(aws_byte_cursor_is_valid(cursor));  
- __CPROVER_assume(cursor->len == 0 || cursor->ptr != NULL);  
- __CPROVER_assume(c_str points to a null‑terminated string);  

Postconditions (validity):  
- The function returns the same boolean value as aws_array_eq_c_str(cursor->ptr, cursor->len, c_str).  

Postconditions (frame):  
- The memory of *cursor, cursor->ptr (if non‑NULL), and c_str must remain unchanged after the call.  
- No additional memory is allocated or freed.  
*/

#include <aws/common/byte_buf.h>
#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_byte_cursor_eq_c_str_harness(void) {
    /* Allocate and nondet‑initialize the cursor */
    struct aws_byte_cursor *cursor = malloc(sizeof(struct aws_byte_cursor));
    __CPROVER_assume(cursor != NULL);
    cursor->len = nondet_size_t();
    if (cursor->len > 0) {
        cursor->ptr = malloc(cursor->len);
        __CPROVER_assume(cursor->ptr != NULL);
        /* Fill buffer with nondet bytes */
        for (size_t i = 0; i < cursor->len; ++i) {
            cursor->ptr[i] = nondet_uint8_t();
        }
    } else {
        cursor->ptr = NULL;
    }

    /* Allocate and nondet‑initialize the C string */
    size_t c_str_len = nondet_size_t();
    char *c_str = malloc(c_str_len + 1);
    __CPROVER_assume(c_str != NULL);
    for (size_t i = 0; i < c_str_len; ++i) {
        c_str[i] = nondet_uint8_t();
    }
    c_str[c_str_len] = '\0'; /* ensure null‑termination */

    /* Snapshot original state */
    struct aws_byte_cursor cursor_snapshot = *cursor;
    uint8_t *ptr_snapshot = NULL;
    if (cursor->ptr != NULL) {
        ptr_snapshot = malloc(cursor->len);
        __CPROVER_assume(ptr_snapshot != NULL);
        memcpy(ptr_snapshot, cursor->ptr, cursor->len);
    }
    char *c_str_snapshot = malloc(c_str_len + 1);
    __CPROVER_assume(c_str_snapshot != NULL);
    memcpy(c_str_snapshot, c_str, c_str_len + 1);

    /* Assume the cursor is valid according to the library's predicate */
    __CPROVER_assume(aws_byte_cursor_is_valid(cursor));

    /* Call the function under test */
    bool result = aws_byte_cursor_eq_c_str(cursor, c_str);

    /* Verify postconditions */
    bool expected = aws_array_eq_c_str(cursor->ptr, cursor->len, c_str);
    assert(result == expected);

    /* Frame conditions: cursor structure unchanged */
    assert(cursor->len == cursor_snapshot.len);
    assert(cursor->ptr == cursor_snapshot.ptr);
    if (cursor->ptr != NULL && ptr_snapshot != NULL) {
        assert(memcmp(cursor->ptr, ptr_snapshot, cursor->len) == 0);
    }

    /* c_str unchanged */
    assert(memcmp(c_str, c_str_snapshot, c_str_len + 1) == 0);

    /* Clean up */
    if (cursor->ptr) free(cursor->ptr);
    free(cursor);
    free(c_str);
    if (ptr_snapshot) free(ptr_snapshot);
    free(c_str_snapshot);

    return 0;
}
