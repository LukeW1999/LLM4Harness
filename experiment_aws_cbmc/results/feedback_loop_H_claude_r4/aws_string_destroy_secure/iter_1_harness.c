#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/**
 * Harness for aws_string_destroy_secure.
 *
 * From the Doxygen:
 *   "Zeroes out the data bytes of string and then deallocates the memory."
 *   "Not safe to run on a string created with AWS_STATIC_STRING_FROM_LITERAL."
 *
 * From the implementation:
 *   if (str) {
 *       aws_secure_zero((void *)aws_string_bytes(str), str->len);
 *       if (str->allocator) {
 *           aws_mem_release(str->allocator, str);
 *       }
 *   }
 *
 * Preconditions:
 *   - str may be NULL (function handles NULL gracefully)
 *   - If str is non-NULL, it must be a valid aws_string
 *   - str must have a non-NULL allocator (otherwise memory won't be freed,
 *     but the function still zeroes the bytes)
 *
 * Postconditions:
 *   - Function returns void; no return value to check
 *   - If str is NULL, nothing happens (no crash)
 *   - If str is non-NULL, the bytes are zeroed and memory is released
 *     (we can only assert no crash / no undefined behavior via CBMC)
 */

/* Helper to allocate a valid aws_string with a given length */
struct aws_string *make_aws_string(size_t len) {
    /* Allocate enough memory for the struct header plus len+1 bytes */
    size_t total_size = sizeof(struct aws_string) + len + 1;
    /* Use __CPROVER_assume to bound allocation */
    __CPROVER_assume(total_size >= sizeof(struct aws_string) + 1);
    
    struct aws_string *str = malloc(total_size);
    if (str == NULL) {
        return NULL;
    }
    
    /* Initialize the const fields via pointer casting (as the library does) */
    struct aws_allocator **allocator_ptr = (struct aws_allocator **)&str->allocator;
    size_t *len_ptr = (size_t *)&str->len;
    
    /* Non-deterministically choose whether to have an allocator */
    if (nondet_bool()) {
        *allocator_ptr = aws_default_allocator();
    } else {
        *allocator_ptr = NULL;
    }
    
    *len_ptr = len;
    
    /* Initialize bytes non-deterministically */
    uint8_t *bytes_ptr = (uint8_t *)str->bytes;
    for (size_t i = 0; i < len; i++) {
        bytes_ptr[i] = nondet_uint8_t();
    }
    bytes_ptr[len] = 0; /* null terminator */
    
    return str;
}

void aws_string_destroy_secure_harness(void) {
    /* Non-deterministically choose to pass NULL or a valid string */
    if (nondet_bool()) {
        /* Test with NULL - should be a no-op */
        aws_string_destroy_secure(NULL);
        /* If we reach here without crashing, the NULL case is handled correctly */
        assert(1); /* Reachability check */
    } else {
        /* Test with a valid string */
        size_t len;
        __CPROVER_assume(len <= 32); /* Bound the string length for tractability */
        
        struct aws_string *str = make_aws_string(len);
        __CPROVER_assume(str != NULL);
        
        /* Verify the string is valid before calling */
        __CPROVER_assume(aws_string_is_valid(str));
        
        /* Save relevant state before the call */
        size_t old_len = str->len;
        struct aws_allocator *old_allocator = str->allocator;
        
        /* Call the function under test */
        aws_string_destroy_secure(str);
        
        /* 
         * After the call:
         * - If allocator was non-NULL, memory has been freed (str is now invalid to dereference)
         * - If allocator was NULL, the bytes were zeroed but memory was not freed
         *   (str is still accessible but bytes should be zero)
         *
         * We can only safely assert postconditions when allocator was NULL
         * (memory not freed, so we can still read the bytes)
         */
        if (old_allocator == NULL) {
            /* Memory was not freed, bytes should have been zeroed */
            const uint8_t *bytes = aws_string_bytes(str);
            for (size_t i = 0; i < old_len; i++) {
                assert(bytes[i] == 0);
            }
            /* len field should be unchanged (we only zeroed bytes, not the struct header) */
            assert(str->len == old_len);
            assert(str->allocator == old_allocator);
        }
        
        /* Function returns void - if we reach here without crashing, basic contract holds */
        assert(1); /* Reachability check */
    }
}
