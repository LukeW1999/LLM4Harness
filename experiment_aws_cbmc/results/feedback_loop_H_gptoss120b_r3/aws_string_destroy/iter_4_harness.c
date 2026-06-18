#include <proof_helpers/make_common_data_structures.h>

#define MAX_STRING_LEN 256

void aws_string_destroy_harness(void) {
    struct aws_string *str;

    /* nondeterministically decide whether str is NULL */
    if (nondet_bool()) {
        str = NULL;
    } else {
        /* allocate a buffer on the stack large enough for the header plus the flexible array */
        uint8_t storage[sizeof(struct aws_string) + MAX_STRING_LEN];
        str = (struct aws_string *)storage;

        /* nondeterministically choose a length bounded by MAX_STRING_LEN */
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_STRING_LEN);

        /* nondeterministically decide whether the string has an allocator */
        if (nondet_bool()) {
            str->allocator = NULL;                     /* static string */
        } else {
            str->allocator = aws_default_allocator(); /* dynamically allocated */
        }

        str->len = len;

        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* Save a shallow copy of the original fields for later comparison */
    struct {
        struct aws_allocator *allocator;
        size_t len;
    } old = {0};

    if (str != NULL) {
        old.allocator = str->allocator;
        old.len = str->len;
    }

    /* Call the function under test */
    aws_string_destroy(str);

    /* Post‑condition checks */

    /* If the string was NULL or had no allocator, the function must not modify the structure */
    if (str == NULL || old.allocator == NULL) {
        if (str != NULL) {
            assert(str->allocator == old.allocator);
            assert(str->len == old.len);
            assert(aws_string_is_valid(str));
        }
    } else {
        /* If the string had a non‑NULL allocator, the memory is released.
         * We must not dereference `str` after this point. */
        assert(old.allocator != NULL);
    }

    /* The validity invariant must hold for any string that was not freed */
    if (str != NULL && old.allocator == NULL) {
        assert(aws_string_is_valid(str));
    }
}
