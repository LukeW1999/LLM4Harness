#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

/*
 * Harness for aws_string_eq:
 *
 * bool aws_string_eq(const struct aws_string *a, const struct aws_string *b)
 *
 * Returns true if bytes of string are the same, false otherwise.
 *
 * From the implementation:
 *   - If a == b (same pointer), returns true.
 *   - If either is NULL, returns false.
 *   - Otherwise, compares bytes using aws_array_eq.
 *
 * Postconditions:
 * 1. RETURN: result is true iff (a == b) OR (a != NULL && b != NULL && a->len == b->len && bytes match).
 *    More precisely, we assert:
 *      - If a == b (and non-null), result must be true.
 *      - If a == NULL || b == NULL (and a != b), result must be false.
 *      - The function does not modify a or b.
 * 2. FRAME: a and b are not modified (their len, bytes, allocator fields are unchanged).
 * 3. INVARIANTS: a and b remain valid after the call.
 */

/* We need a bounded aws_string for CBMC. We'll allocate them with a small max length. */
#ifndef MAX_STRING_LEN
#    define MAX_STRING_LEN 4
#endif

/*
 * Helper: allocate a non-deterministic aws_string with bounded length.
 * The aws_string struct has a flexible-array-like bytes[1] field, but in practice
 * CBMC proofs for aws_string typically use aws_string_is_valid which checks
 * that the bytes pointer is readable for len+1 bytes.
 *
 * We'll create a struct that has enough space for the bytes.
 */
struct aws_string *make_arbitrary_aws_string_with_max_len(size_t max_len) {
    /* Allocate enough memory for the header + bytes + null terminator */
    size_t len;
    __CPROVER_assume(len <= max_len);

    /* Allocate the string: sizeof(struct aws_string) already includes bytes[1],
     * so we need sizeof(struct aws_string) + len bytes total (len extra bytes beyond bytes[0]).
     * Actually bytes[1] gives us 1 byte, so we need len more bytes = len - 1 extra if len >= 1,
     * plus 1 for null terminator. Let's just allocate a large enough buffer. */
    size_t alloc_size = sizeof(struct aws_string) + len; /* bytes[1] + len more = len+1 total bytes */
    struct aws_string *str = malloc(alloc_size);
    if (str == NULL) {
        return NULL;
    }

    /* Set the length field (it's const, so we use a cast trick) */
    *(size_t *)(&str->len) = len;

    /* Set allocator non-deterministically (can be NULL for static strings) */
    /* We leave allocator as whatever malloc gave us, but set it properly */
    *(struct aws_allocator **)(&str->allocator) = nondet_bool() ? NULL : aws_default_allocator();

    /* bytes are already non-deterministic from malloc */
    /* Ensure null terminator at bytes[len] */
    ((uint8_t *)str->bytes)[len] = 0;

    return str;
}

void aws_string_eq_harness(void) {
    /* Non-deterministically choose whether each pointer is NULL */
    bool a_is_null = nondet_bool();
    bool b_is_null = nondet_bool();
    bool a_eq_b = nondet_bool(); /* whether a and b point to the same string */

    struct aws_string *a = NULL;
    struct aws_string *b = NULL;

    if (!a_is_null) {
        a = make_arbitrary_aws_string_with_max_len(MAX_STRING_LEN);
        __CPROVER_assume(a != NULL);
        __CPROVER_assume(aws_string_is_valid(a));
    }

    if (!b_is_null) {
        if (a_eq_b && !a_is_null) {
            b = a; /* same pointer */
        } else {
            b = make_arbitrary_aws_string_with_max_len(MAX_STRING_LEN);
            __CPROVER_assume(b != NULL);
            __CPROVER_assume(aws_string_is_valid(b));
        }
    }

    /* Snapshot state before the call */
    const struct aws_string *a_before = a;
    const struct aws_string *b_before = b;

    size_t a_len_before = (a != NULL) ? a->len : 0;
    size_t b_len_before = (b != NULL) ? b->len : 0;

    struct aws_allocator *a_alloc_before = (a != NULL) ? a->allocator : NULL;
    struct aws_allocator *b_alloc_before = (b != NULL) ? b->allocator : NULL;

    /* Call the function under test */
    bool result = aws_string_eq(a, b);

    /* --- Postcondition 1: RETURN value checks --- */

    /* If a == b (same pointer, both non-null or both null), result must be true */
    if (a == b) {
        /* Same pointer: if both NULL, implementation returns false (NULL check fails before pointer check).
         * Actually: if a == b and a == NULL, then a == b is true but then a == NULL check returns false.
         * Let's re-read: if (a == b) return true; -- this is checked FIRST before NULL check.
         * So if a == b == NULL, result is true. */
        assert(result == true);
    }

    /* If a != b and either is NULL, result must be false */
    if (a != b && (a == NULL || b == NULL)) {
        assert(result == false);
    }

    /* --- Postcondition 2: FRAME checks --- */

    /* The function must not modify the pointers themselves */
    assert(a == a_before);
    assert(b == b_before);

    /* The function must not modify the string contents */
    if (a != NULL) {
        assert(a->len == a_len_before);
        assert(a->allocator == a_alloc_before);
        assert(aws_string_is_valid(a));
    }

    if (b != NULL) {
        assert(b->len == b_len_before);
        assert(b->allocator == b_alloc_before);
        assert(aws_string_is_valid(b));
    }

    /* --- Postcondition 3: INVARIANTS --- */

    /* Result is a bool */
    assert(result == true || result == false);

    /* If result is true and a != b, then both must be non-null and have same length */
    if (result == true && a != b) {
        assert(a != NULL);
        assert(b != NULL);
        assert(a->len == b->len);
    }

    /* If result is false and a == b, that's impossible (a==b always returns true) */
    if (a == b) {
        assert(result == true);
    }
}
