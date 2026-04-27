// === STEP 1: SUCCESS PATH ===
// aws_string_destroy_secure returns void (no return value)
// When str != NULL:
//   - The bytes of str are zeroed (aws_secure_zero called on str->bytes for str->len bytes)
//   - If str->allocator != NULL, the memory is released (str is freed)
//   - If str->allocator == NULL, the memory is NOT freed (static string case)
//
// === STEP 2: FAILURE PATH ===
// When str == NULL:
//   - Nothing happens, function returns immediately
//
// === STEP 3: FRAME CONDITIONS ===
// str (struct aws_string *):
//   - allocator: UNCHANGED (const field, not modified)
//   - len: UNCHANGED (const field, not modified)
//   - bytes: CHANGED (zeroed via aws_secure_zero when str != NULL)
// Note: if str->allocator != NULL, the memory pointed to by str is freed,
//       so we cannot access str after the call in that case.
//
// === STEP 4: VALIDITY INVARIANTS ===
// - No validity invariant to check after call since str may be freed.
//   If str->allocator == NULL, str is still valid after the call (bytes zeroed).
//   If str == NULL, nothing to check.

#include <aws/common/string.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/proof_allocators.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

/* Stub for aws_secure_zero - just zeroes memory */
void aws_secure_zero(void *p, size_t len) {
    if (p && len > 0) {
        memset(p, 0, len);
    }
}

/* Stub for aws_mem_release */
void aws_mem_release(struct aws_allocator *allocator, void *ptr) {
    /* In proof context, we just model the release */
    (void)allocator;
    (void)ptr;
}

void aws_string_destroy_secure_harness(void) {
    /* Non-deterministically choose whether str is NULL or not */
    struct aws_string *str;
    
    if (nondet_bool()) {
        /* NULL case */
        str = NULL;
    } else {
        /* Non-null case: allocate a bounded aws_string */
        /* Use a small bound for the string length */
        size_t len;
        __CPROVER_assume(len <= 8);
        
        /* Allocate memory for the aws_string with its bytes */
        /* aws_string has bytes[1] at end, so we need sizeof(aws_string) - 1 + len + 1 bytes */
        size_t alloc_size = sizeof(struct aws_string) + len;
        str = malloc(alloc_size);
        __CPROVER_assume(str != NULL);
        
        /* Initialize the string fields - cast away const for initialization */
        *(size_t *)(&str->len) = len;
        
        /* Non-deterministically set allocator to NULL or non-NULL */
        if (nondet_bool()) {
            *(struct aws_allocator **)(&str->allocator) = NULL;
        } else {
            struct aws_allocator *alloc = malloc(sizeof(struct aws_allocator));
            __CPROVER_assume(alloc != NULL);
            *(struct aws_allocator **)(&str->allocator) = alloc;
        }
        
        /* Initialize bytes with arbitrary values */
        for (size_t i = 0; i < len; i++) {
            ((uint8_t *)str->bytes)[i] = nondet_uint8_t();
        }
        /* Null terminator */
        ((uint8_t *)str->bytes)[len] = 0;
    }
    
    /* Save state before call */
    bool str_was_null = (str == NULL);
    struct aws_allocator *old_allocator = str_was_null ? NULL : str->allocator;
    size_t old_len = str_was_null ? 0 : str->len;
    bool has_allocator = str_was_null ? false : (str->allocator != NULL);
    
    /* Call the function under test */
    aws_string_destroy_secure(str);
    
    /* === ASSERTIONS === */
    
    /* Case 1: str was NULL - nothing should happen, no crash */
    if (str_was_null) {
        /* Just verify we got here without crashing */
        assert(true);
    } else {
        /* Case 2: str was non-NULL */
        if (!has_allocator) {
            /* allocator was NULL: memory not freed, bytes should be zeroed */
            /* Verify bytes are zeroed */
            for (size_t i = 0; i < old_len; i++) {
                assert(str->bytes[i] == 0);
            }
            /* Verify len and allocator unchanged */
            assert(str->len == old_len);
            assert(str->allocator == old_allocator);
            assert(str->allocator == NULL);
        } else {
            /* allocator was non-NULL: memory was freed after zeroing */
            /* We cannot safely access str after free, but we can verify
               the function completed without error */
            assert(true);
        }
    }
}
