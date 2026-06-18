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
 * 3. Failure: if str is NULL, the function is a no-op (no crash).
 * 4. Validity invariants: after the call, we can only assert things about
 *    the NULL case (since the non-NULL case frees memory).
 *
 * The function:
 *   - If str is NULL: does nothing.
 *   - If str is non-NULL:
 *     - Calls aws_secure_zero on the bytes (zeroes str->len bytes).
 *     - If str->allocator is non-NULL: calls aws_mem_release to free.
 *     - If str->allocator is NULL: does NOT free (static string).
 *
 * For CBMC, we need to create a valid aws_string. We use a bounded allocation.
 */

/* Maximum string length for bounded verification */
#ifndef MAX_STRING_LEN
#define MAX_STRING_LEN 10
#endif

void aws_string_destroy_secure_harness(void) {
    /* Non-deterministically choose whether to pass NULL or a valid string */
    bool is_null = nondet_bool();

    if (is_null) {
        /* Test NULL path: function should be a no-op */
        aws_string_destroy_secure(NULL);
        /* No crash, no assertions needed beyond reaching this point */
        assert(true);
    } else {
        /* Test non-NULL path: create a valid aws_string */
        
        /* Choose a bounded length */
        size_t len;
        __CPROVER_assume(len <= MAX_STRING_LEN);
        
        /* Allocate memory for the string header + data bytes + null terminator */
        /* struct aws_string has bytes[1], so we need len extra bytes beyond that */
        size_t total_size = sizeof(struct aws_string) + len;
        /* Ensure no overflow */
        __CPROVER_assume(total_size >= sizeof(struct aws_string));
        __CPROVER_assume(total_size >= len);
        
        struct aws_string *str = malloc(total_size);
        __CPROVER_assume(str != NULL);
        
        /* Initialize the string fields */
        /* The fields are const, so we use a trick to initialize them */
        /* We write through a non-const pointer to the memory */
        struct aws_string *mutable_str = str;
        
        /* Set len field */
        *(size_t *)(&mutable_str->len) = len;
        
        /* Non-deterministically choose allocator: NULL (static) or real allocator */
        bool has_allocator = nondet_bool();
        if (has_allocator) {
            *(struct aws_allocator **)(&mutable_str->allocator) = aws_default_allocator();
        } else {
            *(struct aws_allocator **)(&mutable_str->allocator) = NULL;
        }
        
        /* Initialize bytes non-deterministically */
        /* bytes[0] is part of the struct, additional bytes follow */
        /* The bytes are already allocated via malloc, CBMC treats them as non-det */
        
        /* Verify the string is valid before calling */
        __CPROVER_assume(aws_string_is_valid(str));
        
        /* Save relevant state before the call */
        struct aws_allocator *old_allocator = str->allocator;
        size_t old_len = str->len;
        
        /* Call the function under test */
        aws_string_destroy_secure(str);
        
        /* 
         * After the call:
         * - If allocator was non-NULL: memory was freed (str is now invalid to dereference)
         * - If allocator was NULL: memory was NOT freed (str still points to valid memory,
         *   but bytes have been zeroed)
         *
         * For the case where allocator is NULL (static string), we can check the bytes.
         * For the case where allocator is non-NULL, we cannot safely dereference str.
         */
        if (!has_allocator) {
            /* Memory was not freed, bytes should be zeroed */
            /* We can verify the string's bytes are zeroed */
            const uint8_t *bytes = aws_string_bytes(str);
            /* Check that bytes are zeroed for the length */
            for (size_t i = 0; i < old_len; i++) {
                assert(bytes[i] == 0);
            }
            /* The len and allocator fields should be unchanged */
            assert(str->len == old_len);
            assert(str->allocator == old_allocator);
        }
        /* 
         * For has_allocator == true case: memory is freed, we cannot assert
         * anything about str's contents. The function completed without error.
         * CBMC will verify no memory safety violations occurred during execution.
         */
        assert(true); /* Reached end without crash */
    }
}
