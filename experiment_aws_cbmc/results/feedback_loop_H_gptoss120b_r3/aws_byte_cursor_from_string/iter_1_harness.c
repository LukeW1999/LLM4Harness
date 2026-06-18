#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

#define MAX_STRING_LEN 256

void aws_byte_cursor_from_string_harness(void) {
    /* 1. Declare src pointer, possibly NULL */
    struct aws_string *src;
    if (nondet_bool()) {
        src = NULL;
    } else {
        /* allocate a nondeterministic length string, bounded */
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_STRING_LEN);

        /* allocate memory for struct + flexible array */
        src = malloc(sizeof(struct aws_string) + (len == 0 ? 0 : len - 1));
        __CPROVER_assume(src != NULL);

        /* initialise fields */
        src->allocator = aws_default_allocator();
        src->len = len;

        /* nondeterministically initialise the bytes */
        uint8_t *bytes = (uint8_t *)src->bytes;
        for (size_t i = 0; i < len; ++i) {
            bytes[i] = nondet_uint8_t();
        }

        /* assume the string satisfies its validity predicate */
        __CPROVER_assume(aws_string_is_valid(src));
    }

    /* 2. Save old state of src (if non‑NULL) for immutability checks */
    struct aws_string old_src;
    struct store_byte_from_buffer old_bytes;
    if (src) {
        old_src = *src;
        save_byte_from_array(src->bytes, src->len, &old_bytes);
    }

    /* 3. Call the function under test */
    struct aws_byte_cursor cur = aws_byte_cursor_from_string(src);

    /* 4. Post‑condition checks */
    if (src == NULL) {
        /* when src is NULL the cursor must be zeroed */
        assert(cur.ptr == NULL);
        assert(cur.len == 0);
    } else {
        /* when src is non‑NULL the cursor must reference the string's bytes */
        assert(cur.ptr == src->bytes);
        assert(cur.len == src->len);

        /* fields of src must remain unchanged */
        assert(src->allocator == old_src.allocator);
        assert(src->len == old_src.len);
        assert_byte_from_buffer_matches(src->bytes, &old_bytes);
    }

    /* 5. Validity invariants */
    assert(aws_byte_cursor_is_bounded(&cur, MAX_STRING_LEN));
    if (src) {
        assert(aws_string_is_valid(src));
    }
}
