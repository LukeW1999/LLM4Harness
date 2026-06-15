#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <aws/common/byte_cursor.h>
#include <aws/common/string.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>

/* Maximum size used for bounding the string length */
#ifndef MAX_STRING_LEN
#   define MAX_STRING_LEN MAX_BUFFER_SIZE
#endif

void aws_byte_cursor_from_string_harness(void) {
    /* 1. Choose whether src is NULL or a valid string */
    struct aws_string *src;
    if (nondet_bool()) {
        src = NULL;
    } else {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_STRING_LEN);

        /* Allocate storage for struct aws_string + flexible array */
        uint8_t storage[sizeof(struct aws_string) + MAX_STRING_LEN];
        src = (struct aws_string *)storage;

        src->allocator = aws_default_allocator();
        src->len = len;

        uint8_t *bytes = (uint8_t *)src->bytes;
        for (size_t i = 0; i < len; ++i) {
            bytes[i] = nondet_uint8_t();
        }
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
    if (src) {
        assert(aws_string_is_valid(src));
    }
    assert(aws_byte_cursor_is_bounded(&cursor, MAX_STRING_LEN));
}
