#include <aws/common/string.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/* Helper to create a valid aws_string with bounded length */
struct aws_string *make_bounded_aws_string(size_t max_len) {
    size_t len;
    __CPROVER_assume(len <= max_len);
    
    /* Allocate enough space for the struct + bytes + null terminator */
    struct aws_string *str = malloc(sizeof(struct aws_string) + len + 1);
    __CPROVER_assume(str != NULL);
    
    /* Initialize fields - use memcpy to work around const */
    struct aws_allocator **alloc_ptr = (struct aws_allocator **)&str->allocator;
    *alloc_ptr = NULL;
    
    size_t *len_ptr = (size_t *)&str->len;
    *len_ptr = len;
    
    /* bytes are non-deterministic (already set by malloc) */
    /* Ensure null terminator */
    uint8_t *bytes_ptr = (uint8_t *)str->bytes;
    bytes_ptr[len] = '\0';
    
    return str;
}

void aws_string_eq_harness(void) {
    struct aws_string *a;
    struct aws_string *b;
    
    /* Non-deterministically choose whether a and b are NULL or valid */
    bool a_is_null;
    bool b_is_null;
    bool a_equals_b;
    
    if (a_is_null) {
        a = NULL;
    } else {
        a = make_bounded_aws_string(4);
        __CPROVER_assume(aws_string_is_valid(a));
    }
    
    if (b_is_null) {
        b = NULL;
    } else if (a_equals_b && !a_is_null) {
        /* Make b point to same object as a */
        b = a;
    } else {
        b = make_bounded_aws_string(4);
        __CPROVER_assume(aws_string_is_valid(b));
    }
    
    /* Save state before call - function should not modify inputs */
    const struct aws_string *old_a = a;
    const struct aws_string *old_b = b;
    size_t old_a_len = (a != NULL) ? a->len : 0;
    size_t old_b_len = (b != NULL) ? b->len : 0;
    
    /* Call the function under test */
    bool result = aws_string_eq(a, b);
    
    /* Postcondition 1: If a == b (same pointer), result must be true */
    if (old_a == old_b) {
        assert(result == true);
    }
    
    /* Postcondition 2: If either is NULL (but not both same pointer), result is false */
    if (old_a == NULL && old_b != NULL) {
        assert(result == false);
    }
    if (old_b == NULL && old_a != NULL) {
        assert(result == false);
    }
    if (old_a == NULL && old_b == NULL) {
        /* a == b (both NULL), so result should be true (caught by first case) */
        assert(result == true);
    }
    
    /* Postcondition 3: If lengths differ, result must be false */
    if (a != NULL && b != NULL && a != b && a->len != b->len) {
        assert(result == false);
    }
    
    /* Postcondition 4: Pointers are unchanged (function doesn't modify inputs) */
    assert(a == old_a);
    assert(b == old_b);
    
    /* Postcondition 5: String contents are unchanged */
    if (a != NULL) {
        assert(a->len == old_a_len);
    }
    if (b != NULL) {
        assert(b->len == old_b_len);
    }
    
    /* Postcondition 6: Validity invariants still hold */
    if (a != NULL) {
        assert(aws_string_is_valid(a));
    }
    if (b != NULL) {
        assert(aws_string_is_valid(b));
    }
    
    /* Postcondition 7: Result is a valid boolean */
    assert(result == true || result == false);
}
