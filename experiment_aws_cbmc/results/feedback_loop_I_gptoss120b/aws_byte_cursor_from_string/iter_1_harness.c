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
    /* 1. Declare src pointer, possibly NULL */
    struct aws_string *src;
    if (nondet_bool()) {
        src = NULL;
    } else {
        /* allocate a string with a bounded length */
        size_t len;
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);
        src = malloc(sizeof(struct aws_string) + len);
        __CPROVER_assume(src != NULL);

        /* allocator may be NULL (static string) or a default allocator */
        if (nondet_bool()) {
            src->allocator = NULL;
        } else {
            src->allocator = aws_default_allocator();
        }

        src->len = len;

        /* initialise the flexible array member (len bytes + terminating NUL) */
        uint8_t *bytes = (uint8_t *)(src->bytes);
        for (size_t i = 0; i < len; ++i) {
            bytes[i] = nondet_uint8_t();
        }
        /* null‑terminator (not counted in len) */
        bytes[len] = 0;

        /* assume the string satisfies its validity predicate */
        __CPROVER_assume(aws_string_is_valid(src));
    }

    /* 2. Save old state of src for immutability checks */
    struct aws_string old;
    struct store_byte_from_buffer old_bytes;
    if (src != NULL) {
        old = *src;                     /* copy scalar fields */
        save_byte_from_array(src->bytes, src->len, &old_bytes);
    }

    /* 3. Call function under test */
    struct aws_byte_cursor cur = aws_byte_cursor_from_string(src);

    /* 4. Post‑condition checks */
    if (src == NULL) {
        /* when src is NULL the cursor must be zeroed */
        assert(cur.ptr == NULL);
        assert(cur.len == 0);
    } else {
        /* cursor must reference the string's bytes and have the same length */
        assert(cur.ptr == aws_string_bytes(src));
        assert(cur.len == src->len);

        /* src must remain unchanged */
        assert(src->allocator == old.allocator);
        assert(src->len == old.len);
        assert_byte_from_buffer_matches(src->bytes, &old_bytes);
    }

    /* 5. Validity invariants */
    if (src != NULL) {
        assert(aws_string_is_valid(src));
    }
    /* the cursor points into a bounded region (len ≤ MAX_BUFFER_SIZE) */
    assert(aws_byte_cursor_is_bounded(&cur, MAX_BUFFER_SIZE));
}
