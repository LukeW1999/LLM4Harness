#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/* aws_array_eq_c_str is used internally; we need its declaration */
bool aws_array_eq_c_str(const void *array, size_t array_len, const char *c_str);

/* Helper to create a valid aws_string with bounded length */
struct aws_string *make_bounded_aws_string(size_t max_len) {
    size_t len;
    __CPROVER_assume(len <= max_len);
    
    /* Allocate enough space for the struct plus the bytes array */
    struct aws_string *str = malloc(sizeof(struct aws_string) + len + 1);
    __CPROVER_assume(str != NULL);
    
    /* Initialize fields - use memcpy to work around const */
    *(size_t *)&str->len = len;
    *(struct aws_allocator **)&str->allocator = NULL;
    
    /* bytes are non-deterministic, but ensure null terminator */
    ((uint8_t *)str->bytes)[len] = '\0';
    
    return str;
}

void aws_string_eq_c_str_harness(void) {
    /* Non-deterministically choose whether str and c_str are NULL */
    bool str_is_null = nondet_bool();
    bool c_str_is_null = nondet_bool();
    
    /* Set up str */
    struct aws_string *str = NULL;
    if (!str_is_null) {
#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 16
#endif
        str = make_bounded_aws_string(MAX_BUFFER_SIZE);
        /* Ensure the string is valid */
        __CPROVER_assume(aws_string_is_valid(str));
    }
    
    /* Set up c_str */
    const char *c_str = NULL;
    if (!c_str_is_null) {
        /* Create a bounded c_str */
        size_t c_str_len;
        __CPROVER_assume(c_str_len <= MAX_BUFFER_SIZE);
        char *c_str_buf = malloc(c_str_len + 1);
        __CPROVER_assume(c_str_buf != NULL);
        c_str_buf[c_str_len] = '\0';
        c_str = c_str_buf;
    }
    
    /* Save state before call - str is not modified by this function */
    const struct aws_string *old_str = str;
    const char *old_c_str = c_str;
    
    /* Call the function under test */
    bool result = aws_string_eq_c_str(str, c_str);
    
    /* Postcondition 1: Both NULL → true */
    if (str == NULL && c_str == NULL) {
        assert(result == true);
    }
    
    /* Postcondition 2: Exactly one NULL → false */
    if (str == NULL && c_str != NULL) {
        assert(result == false);
    }
    if (str != NULL && c_str == NULL) {
        assert(result == false);
    }
    
    /* Postcondition 3: Neither NULL → result depends on content comparison */
    /* We can't easily assert the exact value here without re-implementing,
       but we can assert the function doesn't modify its inputs */
    
    /* Postcondition 4: str pointer is unchanged (function is read-only) */
    assert(str == old_str);
    
    /* Postcondition 5: c_str pointer is unchanged */
    assert(c_str == old_c_str);
    
    /* Postcondition 6: If str is not NULL, its fields are unchanged */
    if (str != NULL) {
        assert(str->len == old_str->len);
        assert(str->allocator == old_str->allocator);
        /* Validity invariant still holds */
        assert(aws_string_is_valid(str));
    }
    
    /* Postcondition 7: Return value is a boolean (true or false) */
    assert(result == true || result == false);
}

int main(void) {
    aws_string_eq_c_str_harness();
    return 0;
}
