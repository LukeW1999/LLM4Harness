#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

/**
 * Harness for aws_string_eq
 *
 * From the Doxygen: Returns true if bytes of string are the same, false otherwise.
 *
 * From the implementation:
 * - If a == b (same pointer), returns true
 * - If either is NULL, returns false
 * - Otherwise, compares bytes using aws_array_eq
 *
 * Postconditions:
 * 1. Changed fields: only the return value (bool result)
 * 2. Unchanged fields: both strings a and b are not modified
 * 3. Both return paths: true means bytes are equal, false means they differ
 * 4. Validity invariants: both strings remain valid after the call
 */

/* Helper to allocate a non-deterministic aws_string with bounded length */
struct aws_string *make_arbitrary_aws_string_nondet_len_with_max(size_t max_len) {
    size_t len;
    __CPROVER_assume(len <= max_len);
    
    /* Allocate memory for the string header + bytes + null terminator */
    struct aws_string *str = malloc(sizeof(struct aws_string) + len + 1);
    __CPROVER_assume(str != NULL);
    
    /* Initialize the fields - cast away const for initialization */
    *(struct aws_allocator **)&str->allocator = aws_default_allocator();
    *(size_t *)&str->len = len;
    
    /* The bytes are non-deterministic (already non-det from malloc) */
    /* Ensure null terminator */
    ((uint8_t *)str->bytes)[len] = '\0';
    
    return str;
}

void aws_string_eq_harness(void) {
    /* 1. Declare inputs non-deterministically */
    struct aws_string *a;
    struct aws_string *b;
    
    /* Non-deterministically choose whether pointers are NULL or valid */
    bool a_is_null = nondet_bool();
    bool b_is_null = nondet_bool();
    bool a_equals_b = nondet_bool();
    
    if (a_is_null) {
        a = NULL;
    } else {
        a = make_arbitrary_aws_string_nondet_len_with_max(4);
        __CPROVER_assume(aws_string_is_valid(a));
    }
    
    if (b_is_null) {
        b = NULL;
    } else if (a_equals_b && !a_is_null) {
        /* Make b point to the same string as a */
        b = a;
    } else {
        b = make_arbitrary_aws_string_nondet_len_with_max(4);
        __CPROVER_assume(aws_string_is_valid(b));
    }
    
    /* 2. Save old state for immutability checks */
    /* Save relevant fields before the call */
    size_t old_a_len = (a != NULL) ? a->len : 0;
    size_t old_b_len = (b != NULL) ? b->len : 0;
    
    /* 3. Call function under test */
    bool result = aws_string_eq(a, b);
    
    /* 4. Assert postconditions */
    
    /* Case: same pointer → must return true */
    if (a == b && a != NULL) {
        assert(result == true);
    }
    
    /* Case: either is NULL (but not same pointer, since a==b==NULL would be caught above) */
    if (a == NULL && b == NULL) {
        /* a == b (both NULL), so same pointer case applies → true */
        assert(result == true);
    } else if (a == NULL || b == NULL) {
        /* One is NULL, other is not → false */
        assert(result == false);
    }
    
    /* Immutability: strings are not modified by the comparison */
    if (a != NULL) {
        assert(a->len == old_a_len);
        assert(aws_string_is_valid(a));
    }
    if (b != NULL) {
        assert(b->len == old_b_len);
        assert(aws_string_is_valid(b));
    }
    
    /* If result is true and both are non-NULL and not same pointer,
     * then lengths must be equal (since aws_array_eq checks length first) */
    if (result == true && a != NULL && b != NULL && a != b) {
        assert(a->len == b->len);
    }
    
    /* If lengths differ and both non-NULL, result must be false */
    if (a != NULL && b != NULL && a != b && a->len != b->len) {
        assert(result == false);
    }
}
