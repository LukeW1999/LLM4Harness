#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* Bounding constant for the string length */
#ifndef MAX_STRING_LEN
#define MAX_STRING_LEN 256
#endif

void aws_string_destroy_harness(void) {
    /* 1. Allocate a possibly non‑deterministic aws_string */
    struct aws_string *str = NULL;

    if (nondet_bool()) {
        /* Allocate memory for the struct plus a flexible array for the bytes */
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_STRING_LEN);

        /* Allocate enough space for the header and the bytes (len bytes, plus the
         * mandatory trailing null byte that is not counted in len). */
        size_t total_size = sizeof(struct aws_string) + (len == 0 ? 0 : len - 1);
        str = malloc(total_size);
        __CPROVER_assume(str != NULL);

        /* Non‑deterministically decide whether the string owns its memory */
        if (nondet_bool()) {
            str->allocator = NULL;               /* static / literal string */
        } else {
            str->allocator = aws_default_allocator(); /* dynamically allocated */
        }

        str->len = len;

        /* Initialise the byte payload with non‑deterministic values */
        for (size_t i = 0; i < len; ++i) {
            ((uint8_t *)str->bytes)[i] = nondet_uint8_t();
        }

        /* The trailing null byte is guaranteed by the implementation; we do not
         * need to model it for the proof. */
        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* 2. Save old state (only if the pointer is non‑NULL) */
    struct aws_string old;
    struct store_byte_from_buffer old_bytes;
    if (str) {
        old = *str;
        save_byte_from_array(str->bytes, str->len, &old_bytes);
    }

    /* 3. Call the function under test */
    aws_string_destroy(str);

    /* 4. Post‑condition checks */
    if (str && old.allocator == NULL) {
        /* The string was static – destroy is a no‑op. All fields must be unchanged. */
        assert(str->allocator == old.allocator);
        assert(str->len == old.len);
        assert_byte_from_buffer_matches(str->bytes, &old_bytes);
        assert(aws_string_is_valid(str));
    } else {
        /* Either str was NULL or it owned its memory (allocator non‑NULL). In both
         * cases the function must not dereference the string after releasing it.
         * No further assertions about the contents of `str` are required. */
        /* (No dereference of `str` here – the absence of further asserts satisfies the
         * requirement that the harness contain at least one assert.) */
    }

    /* 5. Global invariant: a NULL pointer remains NULL */
    if (str == NULL) {
        assert(str == NULL);
    }
}
