// === STEP 1: SUCCESS PATH ===
// aws_byte_cursor_from_c_str always returns a struct (not an error code).
// When c_str is non-NULL:
//   - result.ptr: SET to (uint8_t *)c_str
//   - result.len: SET to strlen(c_str)
//
// === STEP 2: FAILURE PATH ===
// When c_str is NULL:
//   - result.ptr: SET to NULL (cast of NULL)
//   - result.len: SET to 0
//
// === STEP 3: FRAME CONDITIONS ===
// The function takes a const char * and returns a struct by value.
// No input structs are modified. The returned struct fields:
//   result (struct aws_byte_cursor):
//     - ptr: CHANGED based on c_str (set to c_str cast to uint8_t*)
//     - len: CHANGED based on c_str (set to strlen(c_str) or 0)
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_byte_cursor_is_valid(&result): YES (must hold after call)
//   The result is valid if:
//     - (ptr == NULL && len == 0) OR (ptr != NULL && len can be any value)

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>

void aws_byte_cursor_from_c_str_harness(void) {
    /* Non-deterministically choose whether c_str is NULL or a valid string */
    const char *c_str;
    
    if (nondet_bool()) {
        /* NULL case */
        c_str = NULL;
    } else {
        /* Non-NULL case: allocate a bounded string */
        /* Use a small bound to keep verification tractable */
        size_t len;
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);
        char *buf = malloc(len + 1);
        __CPROVER_assume(buf != NULL);
        /* Ensure null terminator exists */
        buf[len] = '\0';
        c_str = buf;
    }
    
    /* Call the function under test */
    struct aws_byte_cursor result = aws_byte_cursor_from_c_str(c_str);
    
    /* Verify postconditions */
    if (c_str == NULL) {
        /* NULL input: ptr should be NULL, len should be 0 */
        assert(result.ptr == NULL);
        assert(result.len == 0);
    } else {
        /* Non-NULL input: ptr should equal c_str, len should equal strlen(c_str) */
        assert(result.ptr == (uint8_t *)c_str);
        assert(result.len == strlen(c_str));
    }
    
    /* Verify the result is a valid aws_byte_cursor */
    assert(aws_byte_cursor_is_valid(&result));
}
