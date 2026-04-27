// === STEP 1: SUCCESS PATH ===
// aws_byte_buf_from_c_str returns a struct aws_byte_buf (not an error code).
// When c_str is non-NULL:
//   - result.len: SET to strlen(c_str)
//   - result.capacity: SET to strlen(c_str) (same as len)
//   - result.buffer: SET to (uint8_t *)c_str
//   - result.allocator: SET to NULL
//
// === STEP 2: FAILURE PATH ===
// When c_str is NULL:
//   - result.len: SET to 0
//   - result.capacity: SET to 0
//   - result.buffer: SET to NULL
//   - result.allocator: SET to NULL
//
// === STEP 3: FRAME CONDITIONS ===
// This function takes a const char* and returns a struct by value.
// No input struct parameters are modified.
// Return value (struct aws_byte_buf):
//   - len: CHANGED based on c_str
//   - capacity: CHANGED based on c_str (equals len)
//   - buffer: CHANGED based on c_str
//   - allocator: always NULL
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_byte_buf_is_valid(&result): YES (must hold after call)
//   - result.allocator == NULL: always
//   - result.len == result.capacity: always
//   - if c_str == NULL: result.buffer == NULL && result.len == 0
//   - if c_str != NULL: result.buffer == (uint8_t*)c_str && result.len == strlen(c_str)

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>

void aws_byte_buf_from_c_str_harness(void) {
    /* Non-deterministically choose whether c_str is NULL or a valid string */
    const char *c_str;
    
    /* We need to bound the string length for CBMC to terminate */
    __CPROVER_assume(c_str == NULL || __CPROVER_is_zero_string(c_str));
    
    /* If non-NULL, bound the string length */
    if (c_str != NULL) {
        __CPROVER_assume(__CPROVER_zero_string_length(c_str) <= MAX_BUFFER_SIZE);
    }
    
    /* Call the function under test */
    struct aws_byte_buf result = aws_byte_buf_from_c_str(c_str);
    
    /* === Assertions for NULL case === */
    if (c_str == NULL) {
        assert(result.len == 0);
        assert(result.capacity == 0);
        assert(result.buffer == NULL);
    }
    
    /* === Assertions for non-NULL case === */
    if (c_str != NULL) {
        size_t expected_len = strlen(c_str);
        assert(result.len == expected_len);
        assert(result.capacity == expected_len);
        assert(result.buffer == (uint8_t *)c_str);
    }
    
    /* === Always-true assertions === */
    assert(result.allocator == NULL);
    assert(result.len == result.capacity);
    
    /* === Validity invariant === */
    assert(aws_byte_buf_is_valid(&result));
}
