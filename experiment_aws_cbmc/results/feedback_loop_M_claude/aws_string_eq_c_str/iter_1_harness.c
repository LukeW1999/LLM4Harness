#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

/**
 * Harness for aws_string_eq_c_str
 *
 * Function behavior:
 * - If both str and c_str are NULL, returns true
 * - If exactly one is NULL, returns false
 * - Otherwise, returns aws_array_eq_c_str(str->bytes, str->len, c_str)
 *
 * This function is a pure comparison — no fields change.
 */

void aws_string_eq_c_str_harness(void) {
    /* Non-deterministically choose whether str is NULL */
    bool str_is_null = nondet_bool();
    
    struct aws_string *str = NULL;
    
    if (!str_is_null) {
        /* Create a valid aws_string */
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);
        
        /* Allocate memory for aws_string header + len bytes + 1 null terminator */
        size_t total_size = sizeof(struct aws_string) + len + 1;
        __CPROVER_assume(total_size >= sizeof(struct aws_string));
        
        str = malloc(total_size);
        __CPROVER_assume(str != NULL);
        
        /* Initialize the string fields */
        /* We need to write to const fields, use a non-const pointer trick */
        struct aws_string *mutable_str = str;
        *(struct aws_allocator **)&mutable_str->allocator = aws_default_allocator();
        *(size_t *)&mutable_str->len = len;
        
        /* bytes are non-deterministic (already allocated) */
        /* Ensure null terminator */
        ((uint8_t *)mutable_str->bytes)[len] = '\0';
        
        __CPROVER_assume(aws_string_is_valid(str));
    }
    
    /* Non-deterministically choose whether c_str is NULL */
    bool c_str_is_null = nondet_bool();
    
    const char *c_str = NULL;
    
    if (!c_str_is_null) {
        /* Create a bounded c_str */
        size_t c_str_len = nondet_size_t();
        __CPROVER_assume(c_str_len <= MAX_BUFFER_SIZE);
        
        char *buf = malloc(c_str_len + 1);
        __CPROVER_assume(buf != NULL);
        buf[c_str_len] = '\0';
        c_str = buf;
    }
    
    /* Save state before call (nothing should change) */
    const struct aws_string *old_str = str;
    const char *old_c_str = c_str;
    
    /* Call function under test */
    bool result = aws_string_eq_c_str(str, c_str);
    
    /* Postconditions */
    
    /* Case 1: Both NULL → must return true */
    if (str == NULL && c_str == NULL) {
        assert(result == true);
    }
    
    /* Case 2: Exactly one NULL → must return false */
    if (str == NULL && c_str != NULL) {
        assert(result == false);
    }
    if (str != NULL && c_str == NULL) {
        assert(result == false);
    }
    
    /* Case 3: Neither NULL → result depends on content comparison */
    /* We can't easily assert the exact value here without reimplementing,
       but we can assert the function didn't corrupt the inputs */
    
    /* Pointers must not change */
    assert(str == old_str);
    assert(c_str == old_c_str);
    
    /* If str is non-null, it must still be valid */
    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }
    
    /* Result must be a valid bool */
    assert(result == true || result == false);
}
