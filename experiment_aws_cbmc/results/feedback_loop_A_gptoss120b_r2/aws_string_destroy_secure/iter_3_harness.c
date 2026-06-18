#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define MAX_BUFFER_SIZE 256

void aws_string_destroy_secure_harness() {
    /* 1. Declare a possibly NULL string and bound its length */
    struct aws_string *str = NULL;
    if (nondet_bool()) {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);

        /* Allocate a stack‑based buffer containing the struct and its bytes */
        uint8_t storage[sizeof(struct aws_string) + MAX_BUFFER_SIZE];
        str = (struct aws_string *)storage;

        /* Nondeterministically choose an allocator (NULL or default) */
        if (nondet_bool()) {
            str->allocator = aws_default_allocator();
        } else {
            str->allocator = NULL;
        }

        /* Set length (cast away const) */
        *((size_t *)&str->len) = len;

        /* Initialize the bytes with nondeterministic values */
        uint8_t *bytes = (uint8_t *)aws_string_bytes(str);
        for (size_t i = 0; i < len; ++i) {
            bytes[i] = nondet_uint8_t();
        }

        /* Assume the string is valid before the call */
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
                assert(bytes[i] == 0);
            }
            assert(str->len == old_len);
            assert(str->allocator == NULL);
            assert(str == old_str);
            assert(aws_string_is_valid(str));
        } else {
            /* The string had a dynamic allocator and has been released.
               No further dereferencing of `str` is performed. */
            (void)old_len; /* silence unused‑variable warning */
        }
    } else {
        /* str was NULL – nothing should have happened. */
        assert(str == NULL);
    }
}
