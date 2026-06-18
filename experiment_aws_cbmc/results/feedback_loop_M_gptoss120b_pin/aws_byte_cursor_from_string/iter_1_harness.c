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
    /* 1. Non‑deterministically decide whether src is NULL */
    bool src_is_null = nondet_bool();

    struct aws_string *src = NULL;
    struct aws_allocator *old_allocator = NULL;
    size_t old_len = 0;
    struct store_byte_from_buffer old_bytes = {0};

    if (!src_is_null) {
        /* 2. Allocate a string with bounded length */
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);

        src = malloc(sizeof(struct aws_string) + len);
        __CPROVER_assume(src != NULL);

        /* allocator (can be NULL for static strings) */
        src->allocator = aws_default_allocator();

        /* set the const len field (cast away const) */
        *((size_t *)&src->len) = len;

        /* fill the flexible array with nondet bytes */
        for (size_t i = 0; i < len; ++i) {
            ((uint8_t *)src->bytes)[i] = nondet_uint8_t();
        }

        /* 3. Assume the string is valid */
        __CPROVER_assume(aws_string_is_valid(src));

        /* 4. Save old immutable state for later checks */
        old_allocator = src->allocator;
        old_len = src->len;
        save_byte_from_array(src->bytes, src->len, &old_bytes);
    }

    /* 5. Call the function under test */
    struct aws_byte_cursor cursor = aws_byte_cursor_from_string(src);

    /* 6. Post‑condition checks */
    if (src == NULL) {
        /* When src is NULL the cursor must be zeroed */
        assert(cursor.ptr == NULL);
        assert(cursor.len == 0);
    } else {
        /* When src is non‑NULL the cursor must reference the string bytes */
        assert(cursor.ptr == aws_string_bytes(src));
        assert(cursor.len == src->len);
    }

    /* 7. Unchanged fields of src (if it existed) */
    if (src != NULL) {
        assert(src->allocator == old_allocator);
        assert(src->len == old_len);
        assert_byte_from_buffer_matches(src->bytes, &old_bytes);
    }

    /* 8. Validity invariant must still hold */
    if (src != NULL) {
        assert(aws_string_is_valid(src));
    }
}
