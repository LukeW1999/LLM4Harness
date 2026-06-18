#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <aws/common/byte_cursor.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_cursor_from_string_harness(void) {
    /* 1. Declare src pointer (may be NULL or a valid aws_string) */
    struct aws_string *src;

    if (nondet_bool()) {
        src = NULL;
    } else {
        /* allocate a nondeterministic length string, bounded */
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);

        /* allocate memory for the struct plus flexible array member */
        src = malloc(sizeof(struct aws_string) + (len > 0 ? len - 1 : 0));
        __CPROVER_assume(src != NULL);

        /* allocator is NULL for static strings (no deallocation) */
        src->allocator = NULL;

        /* set length */
        *((size_t *)&src->len) = len;   /* const field, cast away const for init */

        /* initialise bytes (nondet) – they are part of the allocated block */
        for (size_t i = 0; i < len; ++i) {
            ((uint8_t *)src->bytes)[i] = nondet_uint8_t();
        }

        /* assume the string satisfies its validity predicate */
        __CPROVER_assume(aws_string_is_valid(src));
    }

    /* 2. Save old state of src (if non‑NULL) */
    struct aws_string old_src;
    if (src) {
        old_src = *src;
    }

    /* 3. Call function under test */
    struct aws_byte_cursor cursor = aws_byte_cursor_from_string(src);

    /* 4. Post‑condition assertions */

    if (src == NULL) {
        /* When src is NULL the returned cursor must be zeroed */
        assert(cursor.ptr == NULL);
        assert(cursor.len == 0);
    } else {
        /* When src is non‑NULL the cursor must reference the string's bytes */
        assert(cursor.ptr == aws_string_bytes(src));
        assert(cursor.len == src->len);
    }

    /* 5. Unchanged fields of src (if it existed) */
    if (src) {
        assert(src->allocator == old_src.allocator);
        assert(src->len == old_src.len);
        /* The byte contents must remain unchanged */
        for (size_t i = 0; i < src->len; ++i) {
            assert(((uint8_t *)src->bytes)[i] == ((uint8_t *)old_src.bytes)[i]);
        }
    }

    /* 6. Validity invariants for the returned cursor */
    assert(aws_byte_cursor_is_bounded(&cursor, MAX_BUFFER_SIZE));
}
