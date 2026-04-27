// === STEP 1: SUCCESS PATH ===
// aws_string_destroy_secure returns void (no return value).
// When str != NULL:
//   - The bytes of str are zeroed out (aws_secure_zero called on the data)
//   - If str->allocator != NULL, the memory is freed via aws_mem_release
//   - If str->allocator == NULL, the memory is NOT freed (static string)
//
// === STEP 2: FAILURE PATH ===
// When str == NULL:
//   - Nothing happens (early return)
//
// === STEP 3: FRAME CONDITIONS ===
// str (struct aws_string *):
//   - allocator: UNCHANGED (const field, read only)
//   - len: UNCHANGED (const field, read only)
//   - bytes: CHANGED (zeroed out) when str != NULL
// No other parameters.
//
// === STEP 4: VALIDITY INVARIANTS ===
// After the call, if str had no allocator (static), aws_string_is_valid(str) 
// might still hold (bytes zeroed but len unchanged).
// If str had an allocator, memory is freed - we cannot dereference str after.
// We primarily verify: function doesn't crash, handles NULL, zeros bytes correctly.

#include <aws/common/string.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/proof_allocators.h>
#include <string.h>
#include <assert.h>

/* Stub for aws_secure_zero to allow CBMC to reason about it */
/* We rely on the actual implementation being linked or stubbed */

void aws_string_destroy_secure_harness(void) {
    /* Non-deterministically choose whether str is NULL or not */
    struct aws_string *str;
    
    if (nondet_bool()) {
        /* NULL case */
        str = NULL;
        /* Call with NULL - should be a no-op */
        aws_string_destroy_secure(str);
        /* No assertions needed - just verify no crash */
    } else {
        /* Non-NULL case: allocate a valid aws_string */
        /* We need to allocate memory for the struct + bytes */
        size_t len;
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);
        
        /* Allocate memory for aws_string header + len bytes + 1 null terminator */
        /* sizeof(struct aws_string) already includes 1 byte for bytes[1] */
        size_t total_size = sizeof(struct aws_string) + len;
        /* Avoid overflow */
        __CPROVER_assume(total_size >= sizeof(struct aws_string));
        __CPROVER_assume(total_size >= len);
        
        str = bounded_malloc(total_size);
        __CPROVER_assume(str != NULL);
        
        /* Initialize the string fields - need to write through const */
        /* Use a writable view */
        struct {
            struct aws_allocator *allocator;
            size_t len;
            uint8_t bytes[1];
        } *writable = (void *)str;
        
        /* Non-deterministically choose allocator (NULL = static, non-NULL = dynamic) */
        if (nondet_bool()) {
            writable->allocator = NULL;
        } else {
            writable->allocator = can_fail_allocator();
        }
        writable->len = len;
        
        /* Fill bytes with non-deterministic data */
        for (size_t i = 0; i < len; i++) {
            ((uint8_t *)str->bytes)[i] = nondet_uint8_t();
        }
        
        /* Verify the string is valid before the call */
        assert(aws_string_is_valid(str));
        
        /* Save state before call */
        size_t old_len = str->len;
        struct aws_allocator *old_allocator = str->allocator;
        
        /* If allocator is NULL (static string), we can check bytes after */
        bool has_allocator = (str->allocator != NULL);
        
        /* Make a copy of bytes to verify zeroing (only if no allocator) */
        uint8_t *bytes_copy = NULL;
        if (!has_allocator && len > 0) {
            bytes_copy = malloc(len);
            if (bytes_copy != NULL) {
                memcpy(bytes_copy, str->bytes, len);
            }
        }
        
        /* Call the function under test */
        aws_string_destroy_secure(str);
        
        /* Post-call assertions */
        if (!has_allocator) {
            /* Memory was NOT freed, we can still access str */
            /* len should be unchanged (const field) */
            assert(str->len == old_len);
            /* allocator should still be NULL */
            assert(str->allocator == old_allocator);
            /* bytes should be zeroed */
            if (len > 0) {
                for (size_t i = 0; i < len; i++) {
                    assert(str->bytes[i] == 0);
                }
            }
        }
        /* If has_allocator, memory was freed - cannot dereference str */
        
        if (bytes_copy != NULL) {
            free(bytes_copy);
        }
    }
}
