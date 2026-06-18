#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>

#define MAX_STRING_LEN 256U

void aws_byte_cursor_from_string_harness(void) {
    /* 1. Declare src (may be NULL) and allocate if non‑NULL */
    struct aws_string *src = nondet_pointer();
    if (src) {
        /* stack‑allocated memory for struct + flexible array */
        uint8_t src_mem[sizeof(struct aws_string) + MAX_STRING_LEN];
        src = (struct aws_string *)src_mem;

        /* nondet allocator (NULL for static strings or default) */
        src->allocator = nondet_bool() ? NULL : aws_default_allocator();

        /* nondet length bounded by MAX_STRING_LEN */
        src->len = nondet_size_t();
        __CPROVER_assume(src->len <= MAX_STRING_LEN);

        /* nondet fill the bytes */
        for (size_t i = 0; i < src->len; ++i) {
            src->bytes[i] = nondet_uint8_t();
        }

        /* assume the string satisfies its validity predicate */
        __CPROVER_assume(aws_string_is_valid(src));
    }

    /* 2. Save old state of src (if non‑NULL) */
    struct aws_string old_src;
    struct store_byte_from_buffer old_bytes;
    if (src) {
        old_src = *src;                     /* copies allocator and len */
        save_byte_from_array(src->bytes, src->len, &old_bytes);
    }

    /* 3. Call function under test */
    struct aws_byte_cursor cursor = aws_byte_cursor_from_string(src);

    /* 4. Post‑condition assertions */
    if (src == NULL) {
        /* When src is NULL the cursor is zero‑initialized */
        assert(cursor.ptr == NULL);
        assert(cursor.len == 0);
    } else {
        /* When src is non‑NULL the cursor points to the string bytes */
        assert(cursor.ptr == aws_string_bytes(src));
        assert(cursor.len == src->len);
    }

    /* 5. Unchanged fields of src (if non‑NULL) */
    if (src) {
        assert(src->allocator == old_src.allocator);
        assert(src->len == old_src.len);
        assert_byte_from_buffer_matches(src->bytes, &old_bytes);
    }

    /* 6. Validity invariants */
    if (src) {
        assert(aws_string_is_valid(src));
    }
    /* The cursor should be bounded (no more than MAX_STRING_LEN bytes) */
    assert(aws_byte_cursor_is_bounded(&cursor, MAX_STRING_LEN));
}
