/*  aws_string_destroy_harness.c  */
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <aws/common/string.h>
#include <aws/common/memory.h>
#include "proof_helpers/make_common_data_structures.h"

/* Bounding constant for the flexible array member */
#define MAX_STRING_LEN 256

int main(void) {
    /* 1. Declare a possibly‑null aws_string pointer */
    struct aws_string *str;

    /* nondeterministically decide whether we allocate a string */
    if (nondet_bool()) {
        /* allocate a string with a bounded flexible array */
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_STRING_LEN);

        /* allocate space for the header plus the bytes */
        str = malloc(sizeof(struct aws_string) + len);
        __CPROVER_assume(str != NULL);

        /* nondeterministically decide whether the string has an allocator */
        if (nondet_bool()) {
            /* a dummy allocator – its contents are irrelevant for the harness */
            str->allocator = malloc(1);
            __CPROVER_assume(str->allocator != NULL);
        } else {
            str->allocator = NULL;
        }

        /* set the (const) length field – cast away const for the harness */
        *((size_t *)&str->len) = len;

        /* initialise the byte payload with nondeterministic data */
        for (size_t i = 0; i < len; ++i) {
            ((uint8_t *)str->bytes)[i] = nondet_uint8_t();
        }

        /* 2. Assume the freshly created string satisfies the validity predicate */
        __CPROVER_assume(aws_string_is_valid(str));

        /* 3. Save old state for immutability checks */
        struct aws_string old = *str;
        struct store_byte_from_buffer old_bytes;
        save_byte_from_array(str->bytes, old.len, &old_bytes);
    } else {
        str = NULL;
    }

    /* 4. Call the function under test */
    aws_string_destroy(str);

    /* 5. Post‑condition checks */

    /* -----------------------------------------------------------------
       * If the string was NULL or had a NULL allocator, the function does
       * nothing – the object must remain unchanged.
       * ----------------------------------------------------------------- */
    if (str == NULL || (str != NULL && str->allocator == NULL)) {
        if (str != NULL) {
            /* allocator field unchanged */
            assert(str->allocator == old.allocator);
            /* length field unchanged */
            assert(str->len == old.len);
            /* byte payload unchanged */
            assert_bytes_match(str->bytes, old.bytes, old.len);
        }
        /* validity must still hold */
        if (str != NULL) {
            assert(aws_string_is_valid(str));
        }
    } else {
        /* -----------------------------------------------------------------
           * If both str and str->allocator were non‑NULL, the memory is
           * released.  The pointer variable `str` itself is unchanged, but
           * the object it pointed to is no longer valid – we must not
           * dereference it any further.  No further field‑wise assertions
           * are required.
           * ----------------------------------------------------------------- */
        assert(str != NULL);
        assert(str->allocator != NULL);
        /* The pointer value itself is unchanged (no assignment performed). */
        /* No validity assertion – the object has been freed. */
    }

    return 0;
}
