#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

#define MAX_STRING_BYTES 64

void aws_string_destroy_harness(void) {
    /* 1. Allocate a possibly non‑deterministic aws_string */
    struct aws_string *str = NULL;
    if (nondet_bool()) {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_STRING_BYTES);
        /* allocate space for the struct plus the flexible array member */
        str = malloc(sizeof(struct aws_string) + len);
        __CPROVER_assume(str != NULL);
        /* nondet allocator: either NULL (static string) or a valid allocator */
        if (nondet_bool()) {
            str->allocator = aws_default_allocator();
        } else {
            str->allocator = NULL;
        }
        str->len = len;
        /* initialise the bytes (not required for the proof, but makes the object well‑formed) */
        for (size_t i = 0; i < len; ++i) {
            str->bytes[i] = nondet_uint8_t();
        }
        /* assume the string is valid before the call */
        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* 2. Save old state */
    struct aws_string *old_ptr = str;
    struct aws_string old;
    if (str) {
        old = *str;               /* copy the header (flexible array not fully copied) */
    }

    /* 3. Call function under test */
    aws_string_destroy(str);

    /* 4. Post‑conditions */

    /* The pointer itself must remain unchanged */
    assert(str == old_ptr);

    if (old_ptr && old_ptr->allocator) {
        /* Memory was released; we must not dereference the object any more.
         * No further assertions about its contents are required. */
    } else {
        /* No release happened – the object must be unchanged */
        if (old_ptr) {
            assert(old_ptr->allocator == old.allocator);
            assert(old_ptr->len == old.len);
            /* bytes are unchanged */
            for (size_t i = 0; i < old.len; ++i) {
                assert(old_ptr->bytes[i] == old.bytes[i]);
            }
        }
        /* Validity must still hold */
        assert(aws_string_is_valid(old_ptr));
    }
}
