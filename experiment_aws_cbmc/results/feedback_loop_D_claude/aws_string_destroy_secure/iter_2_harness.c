#include <aws/common/string.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
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
    (void)allocator;
    (void)ptr;
}

void aws_string_destroy_secure_harness(void) {
    struct aws_string *str;
    
    if (nondet_bool()) {
        /* NULL case */
        str = NULL;
    } else {
        /* Non-null case: allocate a bounded aws_string */
        size_t len;
        __CPROVER_assume(len <= 8);
        
        size_t alloc_size = sizeof(struct aws_string) + len;
        str = malloc(alloc_size);
        __CPROVER_assume(str != NULL);
        
        /* Initialize the string fields - cast away const for initialization */
        *(size_t *)(&str->len) = len;
        
        /* Non-deterministically set allocator to NULL or non-NULL */
        if (nondet_bool()) {
            *(struct aws_allocator **)(&str->allocator) = NULL;
        } else {
            struct aws_allocator *alloc = aws_default_allocator();
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
    
    if (str_was_null) {
        /* Just verify we got here without crashing */
        assert(true);
    } else {
        if (!has_allocator) {
            /* allocator was NULL: memory not freed, bytes should be zeroed */
            for (size_t i = 0; i < old_len; i++) {
                assert(str->bytes[i] == 0);
            }
            /* Verify len and allocator unchanged */
            assert(str->len == old_len);
            assert(str->allocator == old_allocator);
            assert(str->allocator == NULL);
        } else {
            /* allocator was non-NULL: memory was freed after zeroing */
            assert(true);
        }
    }
}
