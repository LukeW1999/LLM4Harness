#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_cursor_from_string_harness(void) {
    /* 1. Declare nondeterministic source string pointer */
    struct aws_string *src = nondet_pointer();

    /* 2. If src is non-NULL, allocate and bound its fields */
    if (src) {
        /* allocate enough space for the struct plus maximum possible bytes */
        src = malloc(sizeof(struct aws_string) + MAX_BUFFER_SIZE - 1);
        __CPROVER_assume(src != NULL);

        /* allocator is NULL for static strings (no deallocation) */
        src->allocator = NULL;

        /* nondeterministic length bounded by MAX_BUFFER_SIZE */
        src->len = nondet_size_t();
        __CPROVER_assume(src->len <= MAX_BUFFER_SIZE);

        /* bytes are part of the allocation; no need to initialise contents */
        __CPROVER_assume(aws_string_is_valid(src));

        /* Save old state for immutability checks */
        struct aws_string old = *src;
        struct store_byte_from_buffer old_bytes;
        if (src->len > 0) {
            save_byte_from_array(src->bytes, src->len, &old_bytes);
        }

        /* 3. Call function under test */
        struct aws_byte_cursor cursor = aws_byte_cursor_from_string(src);

        /* 4. Post‑condition when src is non‑NULL */
        assert(cursor.ptr == aws_string_bytes(src));
        assert(cursor.len == src->len);

        /* 5. Immutability checks: src must remain unchanged */
        assert(src->allocator == old.allocator);
        assert(src->len == old.len);
        if (src->len > 0) {
            assert_byte_from_buffer_matches(src->bytes, &old_bytes);
        }

        /* 6. Validity invariants */
        assert(aws_string_is_valid(src));
        assert(aws_byte_cursor_is_bounded(&cursor, MAX_BUFFER_SIZE));
    } else {
        /* 3. Call function under test with NULL source */
        struct aws_byte_cursor cursor = aws_byte_cursor_from_string(NULL);

        /* 4. Post‑condition when src is NULL: cursor must be zeroed */
        assert(cursor.ptr == NULL);
        assert(cursor.len == 0);
    }
}
