// === STEP 1: SUCCESS PATH ===
// When src is non-NULL, aws_byte_cursor_from_string returns a cursor where:
//   - cursor.ptr: points to aws_string_bytes(src) = src->bytes
//   - cursor.len: equals src->len
//
// === STEP 2: FAILURE PATH (NULL src) ===
// When src is NULL, aws_byte_cursor_from_string returns a zeroed cursor:
//   - cursor.ptr: NULL (zeroed)
//   - cursor.len: 0 (zeroed)
//
// === STEP 3: FRAME CONDITIONS ===
// The function returns a new struct aws_byte_cursor by value.
// The input src (struct aws_string *) is not modified.
//   src (struct aws_string *):
//     - allocator: UNCHANGED always
//     - len: UNCHANGED always
//     - bytes: UNCHANGED always
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - The returned cursor should be valid (ptr != NULL or len == 0)
//   - aws_string_is_valid(src): YES (when non-NULL, must hold before call)

#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <assert.h>

struct aws_string *make_arbitrary_aws_string_nondet(size_t max_len) {
    size_t len;
    __CPROVER_assume(len <= max_len);
    // Allocate enough memory for the struct plus len+1 bytes
    struct aws_string *str = malloc(sizeof(struct aws_string) + len + 1);
    __CPROVER_assume(str != NULL);
    // Set the fields (cast away const for initialization)
    *(size_t *)&str->len = len;
    // bytes are already allocated after the struct; no need to set them explicitly
    // allocator can be anything (including NULL)
    return str;
}

void aws_byte_cursor_from_string_harness(void) {
    // Nondeterministically choose whether src is NULL or non-NULL
    struct aws_string *src;
    
    if (nondet_bool()) {
        src = NULL;
    } else {
        // Create a valid aws_string with bounded length
        size_t len;
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);
        
        // Allocate struct + bytes + null terminator
        struct aws_string *str = malloc(sizeof(struct aws_string) + len + 1);
        __CPROVER_assume(str != NULL);
        
        // Initialize fields (cast away const)
        *(size_t *)(&str->len) = len;
        // allocator can be anything
        
        // Ensure aws_string_is_valid holds
        __CPROVER_assume(aws_string_is_valid(str));
        
        src = str;
    }
    
    // Save old state of src (if non-NULL)
    size_t old_len = (src != NULL) ? src->len : 0;
    const uint8_t *old_bytes = (src != NULL) ? src->bytes : NULL;
    struct aws_allocator *old_allocator = (src != NULL) ? src->allocator : NULL;
    
    // Call the function under test
    struct aws_byte_cursor result = aws_byte_cursor_from_string(src);
    
    if (src == NULL) {
        // === STEP 2 assertions: NULL src returns zeroed cursor ===
        assert(result.ptr == NULL);
        assert(result.len == 0);
    } else {
        // === STEP 1 assertions: non-NULL src returns proper cursor ===
        assert(result.ptr == aws_string_bytes(src));
        assert(result.len == src->len);
        
        // === STEP 3 assertions: src is unchanged ===
        assert(src->len == old_len);
        assert(src->bytes == old_bytes);
        assert(src->allocator == old_allocator);
        
        // === STEP 4 assertions: validity invariants ===
        assert(aws_string_is_valid(src));
        // Cursor validity: if len > 0 then ptr != NULL
        if (result.len > 0) {
            assert(result.ptr != NULL);
        }
    }
}
