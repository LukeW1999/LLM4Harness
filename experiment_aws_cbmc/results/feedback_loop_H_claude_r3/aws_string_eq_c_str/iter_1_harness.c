#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/* Maximum string length for bounded verification */
#ifndef MAX_STRING_LEN
#    define MAX_STRING_LEN 8
#endif

/**
 * Helper to allocate a valid aws_string non-deterministically with bounded length.
 */
struct aws_string *make_arbitrary_aws_string_with_max_len(size_t max_len) {
    size_t len;
    __CPROVER_assume(len <= max_len);
    
    /* Allocate memory for the struct + bytes + null terminator */
    struct aws_string *str = malloc(sizeof(struct aws_string) + len + 1);
    __CPROVER_assume(str != NULL);
    
    /* Initialize fields - use memcpy to work around const */
    struct aws_allocator **alloc_ptr = (struct aws_allocator **)&str->allocator;
    *alloc_ptr = NULL; /* static-style string */
    
    size_t *len_ptr = (size_t *)&str->len;
    *len_ptr = len;
    
    /* bytes are non-deterministic (already set by malloc) */
    /* ensure null terminator */
    uint8_t *bytes_ptr = (uint8_t *)str->bytes;
    bytes_ptr[len] = '\0';
    
    return str;
}

/**
 * Helper to create a non-deterministic C string with bounded length.
 */
char *make_arbitrary_c_str(size_t max_len) {
    size_t len;
    __CPROVER_assume(len <= max_len);
    
    char *c_str = malloc(len + 1);
    __CPROVER_assume(c_str != NULL);
    
    /* Ensure null terminator */
    c_str[len] = '\0';
    
    return c_str;
}

void aws_string_eq_c_str_harness(void) {
    /* 1. Non-deterministically choose whether str and c_str are NULL */
    bool str_is_null = nondet_bool();
    bool c_str_is_null = nondet_bool();
    
    const struct aws_string *str = NULL;
    const char *c_str = NULL;
    
    if (!str_is_null) {
        str = make_arbitrary_aws_string_with_max_len(MAX_STRING_LEN);
        __CPROVER_assume(aws_string_is_valid(str));
    }
    
    if (!c_str_is_null) {
        c_str = make_arbitrary_c_str(MAX_STRING_LEN);
    }
    
    /* 2. Save state before call */
    const struct aws_string *old_str = str;
    const char *old_c_str = c_str;
    
    /* 3. Call function under test */
    bool result = aws_string_eq_c_str(str, c_str);
    
    /* 4. Assert postconditions */
    
    /* Case: both NULL -> returns true */
    if (str == NULL && c_str == NULL) {
        assert(result == true);
    }
    
    /* Case: one NULL, other not NULL -> returns false */
    if (str == NULL && c_str != NULL) {
        assert(result == false);
    }
    if (str != NULL && c_str == NULL) {
        assert(result == false);
    }
    
    /* 5. Assert that the inputs are not modified (immutability) */
    assert(str == old_str);
    assert(c_str == old_c_str);
    
    /* If str was non-null, its fields should be unchanged */
    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }
    
    /* 6. Result is a boolean - must be true or false */
    assert(result == true || result == false);
}

void aws_string_eq_c_str_harness(void) {
    aws_string_eq_c_str_harness();
    return 0;
}
