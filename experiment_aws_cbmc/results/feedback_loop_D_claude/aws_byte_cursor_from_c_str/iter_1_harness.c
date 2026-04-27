// === STEP 1: SUCCESS PATH ===
// When c_str is non-NULL:
//   - result.ptr: SET to (uint8_t *)c_str
//   - result.len: SET to strlen(c_str)
//
// === STEP 2: FAILURE PATH (NULL input) ===
// When c_str is NULL:
//   - result.ptr: SET to NULL (0)
//   - result.len: SET to 0
//
// === STEP 3: FRAME CONDITIONS ===
// The function returns a struct by value (no pointer parameters modified).
// result (struct aws_byte_cursor):
//   - ptr: CHANGED - set to c_str cast to uint8_t* (or NULL if c_str is NULL)
//   - len: CHANGED - set to strlen(c_str) if non-NULL, else 0
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_byte_cursor_is_valid(&result): YES (must hold after call)
//   The cursor is valid if:
//     - (ptr == NULL && len == 0) OR (ptr != NULL and ptr[0..len-1] is accessible)

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <string.h>
#include <assert.h>
#include <stddef.h>

#define MAX_STRING_LEN 32

void aws_byte_cursor_from_c_str_harness(void) {
    /* Non-deterministically choose whether c_str is NULL or a valid string */
    char *c_str;
    
    if (nondet_bool()) {
        /* NULL case */
        c_str = NULL;
    } else {
        /* Non-NULL case: allocate a bounded string */
        size_t len;
        __CPROVER_assume(len <= MAX_STRING_LEN);
        c_str = malloc(len + 1);
        __CPROVER_assume(c_str != NULL);
        /* Ensure null terminator exists */
        c_str[len] = '\0';
    }

    /* Call the function under test */
    struct aws_byte_cursor result = aws_byte_cursor_from_c_str(c_str);

    /* === Assertions for Step 1 & 2 === */
    if (c_str != NULL) {
        /* Non-NULL path */
        assert(result.ptr == (uint8_t *)c_str);
        assert(result.len == strlen(c_str));
    } else {
        /* NULL path */
        assert(result.ptr == NULL);
        assert(result.len == 0);
    }

    /* === Step 4: Validity invariant === */
    assert(aws_byte_cursor_is_valid(&result));
}
