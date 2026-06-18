#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_string_destroy_secure_harness() {
    /* 1. Declare a possibly NULL string and bound its length */
    struct aws_string *str = NULL;
    if (nondet_bool()) {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);

        /* allocate space for the struct plus the flexible array member */
        str = malloc(sizeof(struct aws_string) + len);
        __CPROVER_assume(str != NULL);

        /* nondeterministically choose an allocator (NULL or default) */
        if (nondet_bool()) {
            str->allocator = aws_default_allocator();
        } else {
            str->allocator = NULL;
        }

        /* set length */
        *((size_t *)&str->len) = len;   /* cast away const for initialization */

        /* initialize the bytes with nondeterministic values */
        uint8_t *bytes = (uint8_t *)aws_string_bytes(str);
        for (size_t i = 0; i < len; ++i) {
            bytes[i] = nondet_uint8_t();
        }

        /* assume the string is valid before the call */
        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* 2. Save old state */
    struct aws_string *old_str = str;
    size_t old_len = (str) ? str->len : 0;
    struct aws_allocator *old_allocator = (str) ? str->allocator : NULL;

    /* 3. Call the function under test */
    aws_string_destroy_secure(str);

    /* 4. Post‑condition checks */
    if (str) {
        if (old_allocator == NULL) {
            /* The string was statically allocated – it should still be accessible. */
            const uint8_t *bytes = aws_string_bytes(str);
            for (size_t i = 0; i < old_len; ++i) {
                assert(bytes[i] == 0);               /* bytes are zeroed */
            }
            assert(str->len == old_len);             /* length unchanged */
            assert(str->allocator == NULL);           /* allocator unchanged */
            assert(str == old_str);                  /* pointer unchanged */
            assert(aws_string_is_valid(str));         /* validity invariant holds */
        } else {
            /* The string had a dynamic allocator and has been released.
               We must not dereference `str` after this point. No further
               assertions about its contents are made. */
            (void)old_len;   /* silence unused‑variable warning */
        }
    } else {
        /* str was NULL – nothing should have happened. */
        assert(str == NULL);
    }
}
