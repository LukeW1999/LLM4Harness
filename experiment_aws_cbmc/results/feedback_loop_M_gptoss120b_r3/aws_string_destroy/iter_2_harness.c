#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 256
#endif

void aws_string_destroy_harness() {
    /* 1. Declare a possibly NULL aws_string pointer */
    struct aws_string *str = NULL;

    /* nondeterministically decide whether we have a string */
    if (nondet_bool()) {
        /* nondeterministically decide the extra byte payload size, bounded */
        size_t extra_len = nondet_size_t();
        __CPROVER_assume(extra_len <= MAX_BUFFER_SIZE);

        /* allocate memory on the stack for the struct plus the flexible array */
        unsigned char buffer[sizeof(struct aws_string) + MAX_BUFFER_SIZE];
        str = (struct aws_string *)buffer;

        /* set length (bounded by the allocated extra bytes) */
        str->len = extra_len;

        /* nondeterministically decide whether the string is dynamically allocated */
        if (nondet_bool()) {
            str->allocator = aws_default_allocator();
        } else {
            str->allocator = NULL; /* static string – destroy is a no‑op */
        }

        /* The flexible array `bytes` does not need to be initialized for this harness */
    }

    /* 2. Save old state before the call */
    struct aws_string *old_str = str;
    struct aws_allocator *old_allocator = (str != NULL) ? str->allocator : NULL;
    size_t old_len = (str != NULL) ? str->len : 0;

    /* 3. Call the function under test */
    aws_string_destroy(str);

    /* 4. Post‑condition assertions */

    /* If the pointer was NULL, nothing should have happened */
    if (old_str == NULL) {
        assert(str == NULL);
    }
    /* If the string had a NULL allocator, it is a static string – no free occurs */
    else if (old_allocator == NULL) {
        /* The pointer value must remain the same */
        assert(str == old_str);
        /* No fields may have changed */
        assert(str->allocator == NULL);
        assert(str->len == old_len);
        /* Validity invariant must still hold */
        assert(aws_string_is_valid(str));
    }
    /* If the string had a non‑NULL allocator, the memory is released.
       The pointer value itself is unchanged, but the memory it points to is no longer required to be valid.
       Therefore we only assert that the pointer value is unchanged. */
    else {
        assert(str == old_str);
        /* No further dereferencing of `str` is performed after this point. */
    }
}
