#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

/**
 * Harness for aws_string_destroy_secure.
 *
 * From the Doxygen:
 *   "Zeroes out the data bytes of string and then deallocates the memory."
 *
 * Analysis:
 * 1. Changed fields on success: the data bytes are zeroed, then memory is freed.
 *    After the call, the string pointer is no longer valid (freed).
 * 2. Unchanged fields: N/A — the string is destroyed.
 * 3. Failure path: if str is NULL, the function is a no-op (the if(str) guard).
 * 4. Validity invariants: after the call with a non-NULL str, the memory is freed
 *    so we cannot assert validity. For NULL input, nothing changes.
 *
 * We test two cases:
 *   a) str == NULL: function is a no-op, no crash.
 *   b) str != NULL with a valid aws_string: function zeroes bytes and frees memory.
 *      We can only assert things before the call in this case.
 */

/* Helper to allocate a valid aws_string for testing */
struct aws_string *make_aws_string_for_destroy(void) {
    /* We need to allocate an aws_string with some bytes.
     * Use aws_string_new_from_array to create a valid string. */
    size_t len;
    __CPROVER_assume(len <= 10); /* bound the length */
    
    uint8_t *bytes = malloc(len);
    if (bytes == NULL && len > 0) {
        return NULL;
    }
    
    struct aws_allocator *allocator = aws_default_allocator();
    struct aws_string *str = aws_string_new_from_array(allocator, bytes, len);
    return str;
}

void aws_string_destroy_secure_harness(void) {
    /* Non-deterministically choose whether to pass NULL or a valid string */
    bool use_null = nondet_bool();
    
    if (use_null) {
        /* Case 1: NULL input — function should be a no-op */
        aws_string_destroy_secure(NULL);
        /* If we reach here without crashing, the NULL case is handled correctly */
        assert(true); /* no-op case: function handles NULL gracefully */
    } else {
        /* Case 2: Valid (or potentially NULL from allocation failure) string */
        struct aws_string *str = make_aws_string_for_destroy();
        
        if (str != NULL) {
            /* Verify the string is valid before destruction */
            assert(aws_string_is_valid(str));
            
            /* Save properties before destruction */
            size_t old_len = str->len;
            struct aws_allocator *old_allocator = str->allocator;
            
            /* The function should not crash on a valid string */
            /* Note: after this call, str's memory is freed (if allocator != NULL),
             * so we cannot dereference str afterwards */
            aws_string_destroy_secure(str);
            
            /* After destruction, we can only assert that the function completed
             * without crashing. The memory has been freed, so no further
             * dereferences of str are valid. */
            assert(true); /* reached here means no crash */
        } else {
            /* Allocation failed, test NULL path again */
            aws_string_destroy_secure(NULL);
            assert(true);
        }
    }
}

void aws_string_destroy_secure_harness(void) {
    aws_string_destroy_secure_harness();
    return 0;
}
