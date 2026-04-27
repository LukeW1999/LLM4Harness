// === STEP 1: SUCCESS PATH ===
// When aws_string_eq returns true:
//   - Returns true if a == b (same pointer), or if both are non-NULL and their bytes/len match
//   - No struct fields are modified; this is a pure comparison function
//
// === STEP 2: FAILURE PATH ===
// When aws_string_eq returns false:
//   - Returns false if one is NULL and the other isn't, or if bytes differ
//   - No struct fields are modified
//
// === STEP 3: FRAME CONDITIONS ===
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
//   - aws_string_is_valid(a): YES if a was valid before (must hold after call)
//   - aws_string_is_valid(b): YES if b was valid before (must hold after call)
//   - If a == b and both non-NULL, result must be true
//   - If a == NULL xor b == NULL, result must be false
//   - If both NULL, a == b so result is true

#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <assert.h>
#include <string.h>

/* Allocate an aws_string with nondet content of bounded length */
struct aws_string *make_arbitrary_aws_string_with_max_len(size_t max_len) {
    size_t len;
    __CPROVER_assume(len <= max_len);
    /* Allocate enough for the struct plus len bytes plus null terminator */
    struct aws_string *str = malloc(sizeof(struct aws_string) + len + 1);
    if (str == NULL) return NULL;
    /* We need to write to the const fields via a cast */
    size_t *len_ptr = (size_t *)&str->len;
    *len_ptr = len;
    /* bytes are nondet by default from malloc in CBMC */
    /* Ensure null terminator */
    uint8_t *bytes_ptr = (uint8_t *)str->bytes;
    bytes_ptr[len] = 0;
    return str;
}

void aws_string_eq_harness(void) {
    /* Create two aws_string pointers - they may be NULL or valid */
    struct aws_string *a = make_arbitrary_aws_string_with_max_len(4);
    struct aws_string *b = make_arbitrary_aws_string_with_max_len(4);

    /* Nondeterministically make a == b (same pointer) */
    if (nondet_bool()) {
        b = a;
    }

    /* Save old state for frame condition checks */
    size_t old_a_len = (a != NULL) ? a->len : 0;
    size_t old_b_len = (b != NULL) ? b->len : 0;

    /* Save allocator pointers */
    struct aws_allocator *old_a_allocator = (a != NULL) ? a->allocator : NULL;
    struct aws_allocator *old_b_allocator = (b != NULL) ? b->allocator : NULL;

    /* Save bytes content for frame condition */
    uint8_t old_a_bytes[5];
    uint8_t old_b_bytes[5];
    if (a != NULL && a->len <= 4) {
        memcpy(old_a_bytes, a->bytes, a->len + 1);
    }
    if (b != NULL && b->len <= 4) {
        memcpy(old_b_bytes, b->bytes, b->len + 1);
    }

    /* Call the function under test */
    bool result = aws_string_eq(a, b);

    /* === FRAME CONDITIONS: nothing should be modified === */
    if (a != NULL) {
        assert(a->len == old_a_len);
        assert(a->allocator == old_a_allocator);
        if (a->len <= 4) {
            assert(memcmp(a->bytes, old_a_bytes, a->len + 1) == 0);
        }
    }
    if (b != NULL && b != a) {
        assert(b->len == old_b_len);
        assert(b->allocator == old_b_allocator);
        if (b->len <= 4) {
            assert(memcmp(b->bytes, old_b_bytes, b->len + 1) == 0);
        }
    }

    /* === CORRECTNESS ASSERTIONS === */

    /* If a == b (same pointer), result must be true */
    if (a == b) {
        assert(result == true);
    }

    /* If exactly one is NULL, result must be false */
    if (a == NULL && b != NULL) {
        assert(result == false);
    }
    if (a != NULL && b == NULL) {
        assert(result == false);
    }

    /* If both NULL, a == b so already covered above (result == true) */

    /* If both non-NULL and result is true, lengths must match */
    if (a != NULL && b != NULL && result == true) {
        assert(a->len == b->len);
    }

    /* If both non-NULL and lengths differ, result must be false */
    if (a != NULL && b != NULL && a->len != b->len) {
        assert(result == false);
    }
}
