// === STEP 1: SUCCESS PATH ===
// This function always returns a struct aws_byte_buf (no error path).
// When c_str is non-NULL:
//   - buf.len = strlen(c_str)
//   - buf.capacity = strlen(c_str)
//   - buf.buffer = (uint8_t *)c_str  (if len > 0)
//   - buf.allocator = NULL
// When c_str is NULL:
//   - buf.len = 0
//   - buf.capacity = 0
//   - buf.buffer = NULL
//   - buf.allocator = NULL
//
// === STEP 2: FAILURE PATH ===
// No failure path - function always succeeds and returns a struct by value.
//
// === STEP 3: FRAME CONDITIONS ===
// Return value (struct aws_byte_buf):
//   - buf.len: CHANGED (set to strlen(c_str) or 0)
//   - buf.capacity: CHANGED (set to strlen(c_str) or 0)
//   - buf.buffer: CHANGED (set to c_str cast or NULL)
//   - buf.allocator: CHANGED (always set to NULL)
// Input c_str: UNCHANGED (read-only)
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_byte_buf_is_valid(&result): YES (must hold after call)
//   The result should be a valid aws_byte_buf:
//     - len <= capacity
//     - if capacity == 0 then buffer == NULL (or buffer may be non-NULL for empty_array variant, but here capacity==0 => buffer==NULL)
//     - allocator == NULL

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#define MAX_STRING_LEN 32

void aws_byte_buf_from_c_str_harness(void) {
    /* Non-deterministically choose whether c_str is NULL or a valid string */
    char *c_str;
    bool is_null = nondet_bool();
    
    if (is_null) {
        c_str = NULL;
    } else {
        /* Create a bounded non-deterministic string */
        size_t str_len;
        __CPROVER_assume(str_len <= MAX_STRING_LEN);
        c_str = malloc(str_len + 1);
        __CPROVER_assume(c_str != NULL);
        /* Make contents non-deterministic but ensure null termination */
        c_str[str_len] = '\0';
    }
    
    /* Call the function under test */
    struct aws_byte_buf result = aws_byte_buf_from_c_str(c_str);
    
    /* === Assertions for NULL case === */
    if (c_str == NULL) {
        assert(result.len == 0);
        assert(result.capacity == 0);
        assert(result.buffer == NULL);
        assert(result.allocator == NULL);
    } else {
        /* === Assertions for non-NULL case === */
        size_t expected_len = strlen(c_str);
        assert(result.len == expected_len);
        assert(result.capacity == expected_len);
        assert(result.allocator == NULL);
        
        if (expected_len == 0) {
            assert(result.buffer == NULL);
        } else {
            assert(result.buffer == (uint8_t *)c_str);
        }
    }
    
    /* === General validity invariant === */
    assert(result.len <= result.capacity);
    assert(result.allocator == NULL);
    
    /* capacity == 0 implies buffer == NULL */
    if (result.capacity == 0) {
        assert(result.buffer == NULL);
    }
    
    /* aws_byte_buf_is_valid should hold */
    assert(aws_byte_buf_is_valid(&result));
}
