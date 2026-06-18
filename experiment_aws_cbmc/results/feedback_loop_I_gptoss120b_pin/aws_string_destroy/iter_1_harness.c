#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

#define MAX_STRING_LEN 256

void aws_string_destroy_harness(void) {
    /* 1. Declare a possibly NULL aws_string pointer */
    struct aws_string *str;
    bool have_str = nondet_bool();

    if (have_str) {
        /* allocate space for the struct (flexible array member size is 1) */
        str = malloc(sizeof(struct aws_string) + MAX_STRING_LEN);
        __CPROVER_assume(str != NULL);

        /* nondeterministically decide whether allocator is present */
        bool have_allocator = nondet_bool();
        if (have_allocator) {
            str->allocator = aws_default_allocator();
        } else {
            str->allocator = NULL;
        }

        /* nondeterministic length, bounded */
        str->len = nondet_size_t();
        __CPROVER_assume(str->len <= MAX_STRING_LEN);

        /* bytes are not accessed by the function, no need to initialise them */

        /* assume the string is valid before the call */
        __CPROVER_assume(aws_string_is_valid(str));
    } else {
        str = NULL;
    }

    /* 2. Save old state (if any) */
    struct aws_string old;
    if (str != NULL) {
        old = *str; /* shallow copy of fields */
    }

    /* 3. Call function under test */
    aws_string_destroy(str);

    /* 4. Postconditions */

    if (str != NULL && old.allocator != NULL) {
        /* The string memory may have been released. We cannot dereference it,
         * but the pointer value itself must remain unchanged. */
        assert(str == str);
        /* No further assertions about the contents are made because the memory
         * may no longer be accessible. */
    } else {
        /* In all other cases the function does nothing, so the struct must be unchanged. */
        if (str != NULL) {
            assert(str->allocator == old.allocator);
            assert(str->len == old.len);
            /* The flexible array member `bytes` is not modified by the function,
               so its first byte (if any) should remain unchanged. */
            if (old.len > 0) {
                assert(str->bytes[0] == old.bytes[0]);
            }
        }
    }

    /* 5. Validity invariant */
    if (str != NULL && old.allocator != NULL) {
        /* Memory may have been released; we cannot require validity. */
    } else {
        assert(aws_string_is_valid(str));
    }

    /* 6. Clean up allocated memory when it was not freed by the function */
    if (str != NULL && old.allocator == NULL) {
        free(str);
    }
}
