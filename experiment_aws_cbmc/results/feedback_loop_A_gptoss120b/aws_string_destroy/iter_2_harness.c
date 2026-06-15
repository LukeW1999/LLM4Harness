/* aws_string_destroy_harness.c */
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <aws/common/string.h>
#include <aws/common/memory.h>
#include "proof_helpers/make_common_data_structures.h"

/* Bounding constant for the flexible array member */
#define MAX_STRING_LEN 256

int main(void) {
    struct aws_string *str = NULL;

    /* 1. nondeterministically decide whether to allocate a string */
    if (nondet_bool()) {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_STRING_LEN);

        /* allocate space for header + payload */
        str = malloc(sizeof(struct aws_string) + len);
        __CPROVER_assume(str != NULL);

        /* nondeterministically decide whether the string has an allocator */
        if (nondet_bool()) {
            str->allocator = malloc(1);
            __CPROVER_assume(str->allocator != NULL);
        } else {
            str->allocator = NULL;
        }

        /* set length (cast away const) */
        *((size_t *)&str->len) = len;

        /* initialise payload */
        for (size_t i = 0; i < len; ++i) {
            ((uint8_t *)str->bytes)[i] = nondet_uint8_t();
        }

        /* assume the string is valid */
        __CPROVER_assume(aws_string_is_valid(str));

        /* save old state for immutability checks */
        struct aws_string old = *str;
        struct store_byte_from_buffer old_bytes;
        save_byte_from_array(str->bytes, old.len, &old_bytes);
    }

    /* 2. call the function under test */
    aws_string_destroy(str);

    /* 3. post‑condition checks */
    if (str == NULL) {
        /* nothing to check – just ensure no crash */
        assert(str == NULL);
    } else if (str->allocator == NULL) {
        /* allocator was NULL, so the object should remain unchanged */
        assert(str->allocator == NULL);
        assert(str->len == old.len);
        assert_bytes_match(str->bytes, old.bytes, old.len);
        assert(aws_string_is_valid(str));
    } else {
        /* both str and allocator were non‑NULL, memory has been freed */
        /* the pointer variable itself is unchanged */
        assert(str != NULL);
        assert(str->allocator != NULL);
        /* no further dereferencing of `str` is allowed */
    }

    return 0;
}
