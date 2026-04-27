// === STEP 1: SUCCESS PATH ===
// aws_string_eq returns a bool (true/false), not AWS_OP_SUCCESS/ERR
// When aws_string_eq returns true:
//   - a and b have the same bytes and length (or a == b)
//
// === STEP 2: FAILURE PATH ===
// When aws_string_eq returns false:
//   - a or b is NULL, or their bytes differ
//
// === STEP 3: FRAME CONDITIONS ===
// This function is read-only - it does not modify any fields.
// param a (struct aws_string *):
//   - allocator: UNCHANGED always
//   - len: UNCHANGED always
//   - bytes: UNCHANGED always
// param b (struct aws_string *):
//   - allocator: UNCHANGED always
//   - len: UNCHANGED always
//   - bytes: UNCHANGED always
//
// === STEP 4: VALIDITY INVARIANTS ===
// The function handles NULL inputs gracefully (returns false if either is NULL)
// If both are non-NULL, aws_string_is_valid should hold before and after
// No state changes occur, so validity is preserved.

#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdlib.h>
#include <string.h>

/* Maximum string length for bounded verification */
#define MAX_STRING_LEN 10

struct aws_string *make_arbitrary_aws_string_with_bounded_len(size_t max_len) {
    size_t len;
    __CPROVER_assume(len <= max_len);
    
    /* Allocate memory for the aws_string header plus len+1 bytes (for null terminator) */
    struct aws_string *str = malloc(sizeof(struct aws_string) + len + 1);
    __CPROVER_assume(str != NULL);
    
    /* Set the length field (it's const, but we can initialize via pointer cast) */
    *(size_t *)&str->len = len;
    
    /* Set allocator to NULL or some value */
    *(struct aws_allocator **)&str->allocator = NULL;
    
    /* The bytes are already nondet from malloc, just ensure null terminator */
    uint8_t *bytes = (uint8_t *)str->bytes;
    bytes[len] = 0;
    
    return str;
}

void aws_string_eq_harness(void) {
    /* Create two aws_string pointers - they may be NULL */
    struct aws_string *a;
    struct aws_string *b;
    
    /* Nondeterministically choose whether to make them NULL or valid */
    if (nondet_bool()) {
        a = NULL;
    } else {
        a = make_arbitrary_aws_string_with_bounded_len(MAX_STRING_LEN);
    }
    
    if (nondet_bool()) {
        b = NULL;
    } else {
        b = make_arbitrary_aws_string_with_bounded_len(MAX_STRING_LEN);
    }
    
    /* Nondeterministically make a == b (same pointer) */
    if (nondet_bool() && a != NULL) {
        b = a;
    }
    
    /* Save old state before the call */
    size_t old_a_len = (a != NULL) ? a->len : 0;
    size_t old_b_len = (b != NULL) ? b->len : 0;
    
    /* Call the function under test */
    bool result = aws_string_eq(a, b);
    
    /* Verify frame conditions: neither a nor b should be modified */
    if (a != NULL) {
        assert(a->len == old_a_len);
        /* bytes[len] should still be null terminator */
        assert(((uint8_t *)a->bytes)[a->len] == 0);
    }
    
    if (b != NULL) {
        assert(b->len == old_b_len);
        /* bytes[len] should still be null terminator */
        assert(((uint8_t *)b->bytes)[b->len] == 0);
    }
    
    /* Verify correctness properties */
    
    /* If a == b (same pointer), result must be true */
    if (a == b) {
        assert(result == true);
    }
    
    /* If either is NULL (and they're not equal pointers), result must be false */
    if (a == NULL && b != NULL) {
        assert(result == false);
    }
    if (b == NULL && a != NULL) {
        assert(result == false);
    }
    
    /* If both are NULL, a == b so result is true (handled above) */
    
    /* If result is true and both are non-NULL and not same pointer,
     * then lengths must be equal */
    if (result == true && a != NULL && b != NULL && a != b) {
        assert(a->len == b->len);
    }
    
    /* If lengths differ and both non-NULL, result must be false */
    if (a != NULL && b != NULL && a != b && a->len != b->len) {
        assert(result == false);
    }
}
