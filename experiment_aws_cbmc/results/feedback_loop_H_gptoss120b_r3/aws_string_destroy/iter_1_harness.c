#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

#define MAX_STRING_LEN 256

void aws_string_destroy_harness(void) {
    struct aws_string *str;

    /* nondeterministically decide whether str is NULL */
    if (nondet_bool()) {
        str = NULL;
    } else {
        /* allocate a string with a nondeterministic length bounded by MAX_STRING_LEN */
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_STRING_LEN);

        /* allocate enough space for the header plus the flexible array */
        str = (struct aws_string *)malloc(sizeof(struct aws_string) + len);
        __CPROVER_assume(str != NULL);

        /* nondeterministically decide whether the string has an allocator */
        if (nondet_bool()) {
            str->allocator = NULL;                     /* static string */
        } else {
            str->allocator = aws_default_allocator(); /* dynamically allocated */
        }

        str->len = len;

        /* The bytes themselves are not accessed by the destroy function,
         * so we leave them uninitialized. */
        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* Save a shallow copy of the original structure for later comparison.
     * If str is NULL we keep old uninitialized – it will never be used. */
    struct aws_string old = {0};
    if (str != NULL) {
        old = *str;
    }

    /* Call the function under test */
    aws_string_destroy(str);

    /* --------------------------------------------------------------------- *
     * Post‑condition checks
     * --------------------------------------------------------------------- */

    /* If the string was NULL or had no allocator, the function must not
     * modify the structure. */
    if (str == NULL || old.allocator == NULL) {
        if (str != NULL) {
            assert(str->allocator == old.allocator);
            assert(str->len == old.len);
            /* The flexible‑array bytes are not required to be unchanged,
             * but they must still be readable because the string was not
             * freed.  The validity predicate checks this. */
            assert(aws_string_is_valid(str));
        }
    } else {
        /* If the string had a non‑NULL allocator, the memory is released.
         * The pointer `str` now refers to freed memory; we must not dereference
         * it.  Therefore we make no assertions about its fields. */
        /* However, we can still assert that the allocator pointer we passed to
         * aws_mem_release was the one stored in the structure. */
        assert(old.allocator != NULL);
    }

    /* The validity invariant must hold for any string that was not freed. */
    if (str != NULL && old.allocator == NULL) {
        assert(aws_string_is_valid(str));
    }
}
