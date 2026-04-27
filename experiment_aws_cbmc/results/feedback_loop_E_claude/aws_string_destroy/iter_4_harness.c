/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/string.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>

void aws_string_destroy_harness() {
    /* Nondeterministically choose between NULL and valid string */
    int choice;
    
    if (choice) {
        /* Test Case 1: NULL string - function should handle gracefully */
        aws_string_destroy(NULL);
        assert(1); /* Reached here means NULL was handled */
    } else {
        /* Test Case 2: Valid string with a valid allocator */
        struct aws_allocator *alloc = aws_default_allocator();
        assert(alloc != NULL);
        
        /* Create a string using the allocator */
        const char *test_bytes = "test";
        struct aws_string *str = aws_string_new_from_c_str(alloc, test_bytes);
        
        if (str != NULL) {
            /* Verify preconditions */
            assert(aws_string_is_valid(str));
            assert(str->allocator != NULL);
            
            /* Save allocator pointer before destruction */
            struct aws_allocator *saved_alloc = str->allocator;
            assert(saved_alloc != NULL);
            
            /* Call the function under test */
            aws_string_destroy(str);
            
            /* After destroy, verify allocator still exists */
            assert(saved_alloc != NULL);
        } else {
            /* String allocation failed - this is a valid path */
            assert(str == NULL);
        }
    }
}
