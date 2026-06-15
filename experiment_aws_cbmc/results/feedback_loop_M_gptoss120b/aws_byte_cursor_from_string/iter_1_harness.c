#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

/* Helper to allocate an aws_string with a given length */
static struct aws_string *allocate_string_with_len(size_t len) {
    /* Allocate enough space for the struct plus (len-1) extra bytes for the flexible array */
    size_t total_size = sizeof(struct aws_string) + (len > 0 ? len - 1 : 0);
    struct aws_string *s = (struct aws_string *)malloc(total_size);
    __CPROVER_assume(s != NULL);
    s->allocator = NULL;               /* allocator not relevant for this harness */
    s->len = len;
    /* Fill the byte array with nondeterministic data */
    for (size_t i = 0; i < len; ++i) {
        ((uint8_t *)s->bytes)[i] = nondet_uint8_t();
    }
    return s;
}

void aws_byte_cursor_from_string_harness(void) {
    /* 1. nondeterministically decide whether src is NULL */
    struct aws_string *src = nondet_bool() ? NULL : allocate_string_with_len(nondet_size_t());

    if (src != NULL) {
        /* Bound the length of the string */
        __CPROVER_assume(src->len <= MAX_BUFFER_SIZE);
        /* Assume the string satisfies its validity predicate */
        __CPROVER_assume(aws_string_is_valid(src));

        /* Save a snapshot of the string's immutable fields for later comparison */
        struct store_byte_from_buffer old_bytes;
        save_byte_from_array(src->bytes, src->len, &old_bytes);
        const struct aws_allocator *old_allocator = src->allocator;
        const size_t old_len = src->len;
    }

    /* 2. Call the function under test */
    struct aws_byte_cursor cursor = aws_byte_cursor_from_string(src);

    /* 3. Post‑condition checks */
    if (src == NULL) {
        /* When src is NULL the returned cursor must be zero‑initialized */
        assert(cursor.ptr == NULL);
        assert(cursor.len == 0);
    } else {
        /* When src is non‑NULL the cursor must point to the string's bytes and have the same length */
        assert(cursor.ptr == aws_string_bytes(src));
        assert(cursor.len == src->len);

        /* The returned cursor must be bounded */
        assert(aws_byte_cursor_is_bounded(&cursor, MAX_BUFFER_SIZE));

        /* The source string must remain unchanged */
        assert(src->allocator == old_allocator);
        assert(src->len == old_len);
        assert_byte_from_buffer_matches(src->bytes, &old_bytes);
    }

    /* 4. In all cases the returned cursor must be a valid cursor */
    assert(aws_byte_cursor_is_bounded(&cursor, MAX_BUFFER_SIZE));
}
