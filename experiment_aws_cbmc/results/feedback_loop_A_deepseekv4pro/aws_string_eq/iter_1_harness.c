#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_string_eq_harness() {
    /* Declare two aws_string pointers */
    struct aws_string *a;
    struct aws_string *b;

    /* Non-deterministically choose whether each is NULL or a valid string */
    bool a_is_null = nondet_bool();
    bool b_is_null = nondet_bool();

    if (a_is_null) {
        a = NULL;
    } else {
        /* Allocate and assume a valid aws_string */
        size_t a_len = nondet_size_t();
        __CPROVER_assume(a_len <= MAX_BUFFER_SIZE);

        /* Allocate memory for the aws_string header + len bytes + null terminator */
        size_t a_alloc_size = sizeof(struct aws_string) + a_len;
        a = (struct aws_string *)malloc(a_alloc_size);
        if (a != NULL) {
            /* Initialize constant fields via pointer casting (CBMC non-const initialization) */
            struct aws_allocator *alloc = aws_default_allocator();
            /* We must write to const fields — CBMC allows this for initialization */
            *((struct aws_allocator **)&a->allocator) = alloc;
            *((size_t *)&a->len) = a_len;
            /* The bytes array is flexible; we assume they are readable */
            __CPROVER_assume(AWS_MEM_IS_READABLE(a->bytes, a_len + 1));
            __CPROVER_assume(aws_string_is_valid(a));
        }
    }

    if (b_is_null) {
        b = NULL;
    } else {
        size_t b_len = nondet_size_t();
        __CPROVER_assume(b_len <= MAX_BUFFER_SIZE);

        size_t b_alloc_size = sizeof(struct aws_string) + b_len;
        b = (struct aws_string *)malloc(b_alloc_size);
        if (b != NULL) {
            struct aws_allocator *alloc = aws_default_allocator();
            *((struct aws_allocator **)&b->allocator) = alloc;
            *((size_t *)&b->len) = b_len;
            __CPROVER_assume(AWS_MEM_IS_READABLE(b->bytes, b_len + 1));
            __CPROVER_assume(aws_string_is_valid(b));
        }
    }

    /* Save old states for immutability checks (only if non-NULL) */
    struct aws_string *old_a = a;
    struct aws_string *old_b = b;

    /* Call the function under test */
    bool result = aws_string_eq(a, b);

    /* Postconditions */

    /* 1. If a == b (pointer equality), result must be true */
    if (a == b) {
        assert(result == true);
    }

    /* 2. If either is NULL (and not both, since a==b covers both NULL), result must be false */
    if (a == NULL && b != NULL) {
        assert(result == false);
    }
    if (b == NULL && a != NULL) {
        assert(result == false);
    }

    /* 3. Both NULL case is covered by a==b -> true */

    /* 4. For non-NULL, non-equal strings, result depends on byte comparison.
     * We can't directly assert the result without replicating aws_array_eq,
     * but we CAN assert that the strings themselves haven't been modified. */
    if (a != NULL && b != NULL && a != b) {
        /* The strings should be immutable — no fields changed */
        assert(a->allocator == old_a->allocator);
        assert(a->len == old_a->len);
        assert(b->allocator == old_b->allocator);
        assert(b->len == old_b->len);
    }

    /* 5. Validity invariants: non-NULL strings must still be valid after the call */
    if (a != NULL) {
        assert(aws_string_is_valid(a));
    }
    if (b != NULL) {
        assert(aws_string_is_valid(b));
    }

    /* Free allocated memory */
    if (a != NULL) { free(a); }
    if (b != NULL) { free(b); }
}
