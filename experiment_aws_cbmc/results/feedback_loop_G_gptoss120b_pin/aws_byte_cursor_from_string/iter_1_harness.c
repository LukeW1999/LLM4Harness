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
    /* 1. Declare src pointer, nondeterministically NULL or a valid string */
    struct aws_string *src;
    if (nondet_bool()) {
        src = NULL;
    } else {
        /* allocate space for the struct plus a bounded payload */
        size_t max_len = MAX_BUFFER_SIZE;
        src = malloc(sizeof(struct aws_string) + max_len);
        __CPROVER_assume(src != NULL);

        /* nondet length bounded by max_len */
        src->len = nondet_size_t();
        __CPROVER_assume(src->len <= max_len);

        /* allocator (static strings have NULL allocator) */
        src->allocator = aws_default_allocator();

        /* nondet fill of the payload bytes (the flexible array) */
        for (size_t i = 0; i < src->len; ++i) {
            ((uint8_t *)src->bytes)[i] = nondet_uint8_t();
        }

        /* assume the string satisfies its validity predicate */
        __CPROVER_assume(aws_string_is_valid(src));
    }

    /* 2. Save old state of src (if non‑NULL) for immutability checks */
    struct aws_string old;
    struct store_byte_from_buffer old_bytes;
    if (src != NULL) {
        old = *src;
        save_byte_from_array(src->bytes, src->len, &old_bytes);
    }

    /* 3. Call function under test */
    struct aws_byte_cursor cur = aws_byte_cursor_from_string(src);

    /* 4. Post‑condition checks */
    if (src == NULL) {
        /* When src is NULL the cursor must be zero‑initialized */
        assert(cur.ptr == NULL);
        assert(cur.len == 0);
    } else {
        /* When src is non‑NULL the cursor must reference the string's bytes */
        assert(cur.ptr == aws_string_bytes(src));
        assert(cur.len == src->len);

        /* src must remain unchanged */
        assert(src->allocator == old.allocator);
        assert(src->len == old.len);
        assert_byte_from_buffer_matches(src->bytes, &old_bytes);
    }

    /* 5. Validity invariant must still hold for src (if non‑NULL) */
    if (src != NULL) {
        assert(aws_string_is_valid(src));
    }
}
