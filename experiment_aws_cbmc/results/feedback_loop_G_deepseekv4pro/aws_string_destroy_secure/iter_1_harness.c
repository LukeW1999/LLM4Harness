#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_string_destroy_secure_harness() {
    /* 1. Declare and bound data structures */
    struct aws_string *str = NULL;
    
    /* Non-deterministically decide if str is NULL or a valid string */
    bool str_is_null = nondet_bool();
    
    if (!str_is_null) {
        /* Allocate a string with non-deterministic length */
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);
        
        /* Allocate memory for the string header + data bytes + null terminator */
        size_t total_size = sizeof(struct aws_string) + len; /* bytes[1] already accounts for 1 byte */
        str = (struct aws_string *)malloc(total_size);
        __CPROVER_assume(str != NULL);
        
        /* Initialize the string fields */
        /* allocator can be NULL (static string) or a valid allocator */
        bool has_allocator = nondet_bool();
        if (has_allocator) {
            /* We need to write to the const field via pointer manipulation for CBMC */
            *(struct aws_allocator **)&str->allocator = aws_default_allocator();
        } else {
            *(struct aws_allocator **)&str->allocator = NULL;
        }
        
        /* Set the length */
        *(size_t *)&str->len = len;
        
        /* Ensure the bytes are writable (for the zeroing operation) */
        __CPROVER_assume(AWS_MEM_IS_WRITABLE(str->bytes, len));
        __CPROVER_assume(AWS_MEM_IS_READABLE(str->bytes, len));
        
        /* Ensure the string is valid */
        __CPROVER_assume(aws_string_is_valid(str));
    }
    
    /* 2. Save old state BEFORE calling */
    struct aws_string *old_str = str;
    bool old_was_null = (str == NULL);
    struct aws_allocator *old_allocator = NULL;
    size_t old_len = 0;
    
    if (!old_was_null) {
        old_allocator = str->allocator;
        old_len = str->len;
    }
    
    /* 3. Call function under test */
    aws_string_destroy_secure(str);
    
    /* 4. Assert postconditions */
    if (old_was_null) {
        /* When str is NULL, the function is a no-op. Nothing to assert beyond that. */
        assert(str == NULL);
    } else {
        /* When str is non-NULL:
         * - The data bytes have been zeroed out (secure_zero)
         * - If allocator is non-NULL, the memory has been released
         * - If allocator is NULL, the memory is NOT released (static string)
         */
        
        if (old_allocator != NULL) {
            /* Memory was released. We cannot access str anymore.
             * CBMC will verify that no use-after-free occurs.
             * The function guarantees the data was zeroed before release.
             */
            /* No further assertions on str since it's freed */
        } else {
            /* Static string: allocator is NULL, so memory is NOT freed.
             * The data bytes should have been zeroed out.
             * We can still access str and its bytes.
             */
            /* Verify the bytes were zeroed */
            if (old_len > 0) {
                for (size_t i = 0; i < old_len; i++) {
                    assert(str->bytes[i] == 0);
                }
            }
            
            /* Verify the string structure fields are unchanged */
            assert(str->allocator == old_allocator); /* still NULL */
            assert(str->len == old_len);
        }
    }
    
    /* 5. No validity invariant to check since the string is either freed or zeroed */
}
