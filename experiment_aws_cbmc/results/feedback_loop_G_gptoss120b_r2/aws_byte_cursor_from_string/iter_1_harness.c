#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
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
        /* allocate a non‑NULL aws_string with bounded length */
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_STRING_LEN);

        /* allocate enough space for the struct plus the flexible array */
        src = malloc(sizeof(struct aws_string) + len);
        __CPROVER_assume(src != NULL);

        /* initialize fields */
        src->allocator = aws_default_allocator();
        src->len = len;

        /* nondeterministically initialize the bytes */
        uint8_t *bytes = (uint8_t *)src->bytes;
        for (size_t i = 0; i < len; ++i) {
            bytes[i] = nondet_uint8_t();
        }

        /* assume the string satisfies its validity predicate */
        __CPROVER_assume(aws_string_is_valid(src));
    }

    /* 2. Save old state of src (if non‑NULL) for immutability checks */
    struct aws_string old_src;
    if (src) {
        old_src = *src;               /* copies allocator and len (bytes are not copied) */
    }

    /* 3. Call function under test */
    struct aws_byte_cursor cursor = aws_byte_cursor_from_string(src);

    /* 4. Post‑condition checks */
    if (src == NULL) {
        /* When src is NULL the function must return an empty cursor */
        assert(cursor.ptr == NULL);
        assert(cursor.len == 0);
    } else {
        /* When src is non‑NULL the cursor must reference the string's bytes */
        assert(cursor.ptr == aws_string_bytes(src));
        assert(cursor.len == src->len);

        /* src must remain unchanged */
        assert(src->allocator == old_src.allocator);
        assert(src->len == old_src.len);
        /* the pointer to the bytes must be unchanged */
        assert(cursor.ptr == src->bytes);
    }

    /* 5. Validity invariants */
    /* The returned cursor must be bounded (no overflow) */
    assert(aws_byte_cursor_is_bounded(&cursor, MAX_STRING_LEN));

    if (src) {
        /* The source string must still be valid */
        assert(aws_string_is_valid(src));
    }
}
