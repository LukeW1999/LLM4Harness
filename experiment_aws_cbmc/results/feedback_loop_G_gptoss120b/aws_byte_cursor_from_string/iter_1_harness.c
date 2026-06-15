#include <stddef.h>
#include <stdlib.h>
#include <aws/common/string.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_byte_cursor_from_string_harness(void) {
    /* 1. Declare src pointer, possibly NULL */
    struct aws_string *src;
    bool src_is_null = nondet_bool();

    if (src_is_null) {
        src = NULL;
    } else {
        /* bound length */
        size_t max_len = MAX_BUFFER_SIZE;
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= max_len);

        /* allocate struct with flexible array */
        src = malloc(sizeof(struct aws_string) + len);
        __CPROVER_assume(src != NULL);

        /* nondet allocator (could be NULL) */
        src->allocator = (struct aws_allocator *)nondet_uint8_t(); /* just a placeholder */

        src->len = len;

        /* fill the flexible array with nondet data */
        uint8_t *bytes = (uint8_t *)src->bytes;
        for (size_t i = 0; i < len; ++i) {
            bytes[i] = nondet_uint8_t();
        }
        /* optional null terminator (not counted in len) */
        if (len < max_len) {
            bytes[len] = 0;
        }

        __CPROVER_assume(aws_string_is_valid(src));
    }

    /* 2. Save old state of src (if non‑NULL) */
    struct aws_string old_src;
    if (src != NULL) {
        old_src = *src; /* shallow copy; bytes are compared later */
    }

    /* 3. Call function under test */
    struct aws_byte_cursor cursor = aws_byte_cursor_from_string(src);

    /* 4. Post‑condition assertions */

    /* When src is NULL the returned cursor must be zeroed */
    if (src == NULL) {
        assert(cursor.ptr == NULL);
        assert(cursor.len == 0);
    } else {
        /* When src is non‑NULL the cursor must reference the string's bytes */
        assert(cursor.ptr == aws_string_bytes(src));
        assert(cursor.len == src->len);
    }

    /* 5. src must remain unchanged */
    if (src != NULL) {
        assert(src->allocator == old_src.allocator);
        assert(src->len == old_src.len);
        for (size_t i = 0; i < src->len; ++i) {
            assert(((uint8_t *)src->bytes)[i] == ((uint8_t *)old_src.bytes)[i]);
        }
    }

    /* 6. Validity invariants */
    assert(aws_byte_cursor_is_bounded(&cursor, MAX_BUFFER_SIZE));
}
