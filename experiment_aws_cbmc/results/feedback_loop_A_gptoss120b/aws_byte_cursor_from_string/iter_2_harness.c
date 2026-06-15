#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <aws/common/byte_cursor.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/assertions.h>

/* Maximum size used for bounding the string length */
#ifndef MAX_STRING_LEN
#   define MAX_STRING_LEN MAX_BUFFER_SIZE
#endif

void aws_byte_cursor_from_string_harness(void) {
    /* 1. Choose whether src is NULL or a valid string */
    const struct aws_string *src;
    if (nondet_bool()) {
        src = NULL;
    } else {
        /* Allocate a string with nondeterministic length */
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_STRING_LEN);

        struct aws_string *tmp = malloc(sizeof(struct aws_string) + len);
        __CPROVER_assume(tmp != NULL);

        /* Nondeterministic allocator (may be NULL for static strings) */
        tmp->allocator = (struct aws_allocator *)malloc(1);
        tmp->len = len;

        /* Fill the flexible array member with nondet bytes */
        uint8_t *bytes = (uint8_t *)tmp->bytes;
        for (size_t i = 0; i < len; ++i) {
            bytes[i] = nondet_uint8_t();
        }

        /* Assume the constructed string satisfies the library invariant */
        __CPROVER_assume(aws_string_is_valid(tmp));

        src = tmp;
    }

    /* 2. Record old state of src for immutability checks */
    struct aws_string old_src;
    struct store_byte_from_buffer old_bytes;
    if (src) {
        old_src = *src; /* shallow copy of allocator and len */
        save_byte_from_array(src->bytes, src->len, &old_bytes);
    }

    /* 3. Call the function under test */
    struct aws_byte_cursor cursor = aws_byte_cursor_from_string(src);

    /* 4. Post‑condition checks */
    if (src == NULL) {
        /* When src is NULL the function should return a zero‑initialized cursor */
        assert(cursor.ptr == NULL);
        assert(cursor.len == 0);
    } else {
        /* Cursor must reference the string's bytes */
        assert(cursor.ptr == aws_string_bytes(src));
        assert(cursor.len == src->len);

        /* Source string must remain unchanged */
        assert(src->allocator == old_src.allocator);
        assert(src->len == old_src.len);
        assert_byte_from_buffer_matches(src->bytes, &old_bytes);
    }

    /* 5. Global invariants that must always hold */
    assert(aws_string_is_valid(src));
    assert(aws_byte_cursor_is_bounded(&cursor, MAX_STRING_LEN));
}
